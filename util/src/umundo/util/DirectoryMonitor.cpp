#include "umundo/util/DirectoryMonitor.h"

namespace umundo {

DirectoryMonitor::DirectoryMonitor(string directory, string channelName) : 
	_directory(directory), _channelName(channelName), _lastChecked(0) 
{
	_pub = new Publisher(_channelName);
	_pub->setGreeter(new DirectoryMonitorGreeter(this));
}

set<Publisher*> DirectoryMonitor::getPublishers() {
	set<Publisher*> pubs;
	pubs.insert(_pub);
	return pubs;
}


void DirectoryMonitor::DirectoryMonitorGreeter::welcome(Publisher* pub, const string nodeId, const string subId) {
	_monitor->_mutex.lock();
	map<string, struct stat>::iterator fileIter = _monitor->_knownEntries.begin();
	while(fileIter != _monitor->_knownEntries.end()) {

		int size = fileIter->second.st_size;
		char* buffer = _monitor->readFileIntoBuffer(fileIter->first, size);

		Message* msg = Message::toSubscriber(subId);
		msg->setMeta("file", fileIter->first);
		msg->setMeta("directory", _monitor->_directory);
		msg->setMeta("operation", "added");
		msg->setData(buffer, size);

		LOG_DEBUG("Publishing %d bytes from file %s", size, fileIter->first.c_str());

		_monitor->_pub->send(msg);
		fileIter++;
		delete msg;
	}
	_monitor->_mutex.unlock();
}

void DirectoryMonitor::run() {

	while(isStarted()) {
		// stat directory for modification date
	  struct stat dirStat;
		if (stat(_directory.c_str(), &dirStat) != 0) {
    	LOG_ERR("Error with stat on directory '%s': %s", _directory.c_str(), strerror(errno));
			return;
		}

		while(_pub->waitForSubscribers(0) == 0) {
			LOG_DEBUG("Not publishing as none is listening");
			_pub->waitForSubscribers(1);
		}


		if (dirStat.st_mtime >= _lastChecked) {
			// there are changes in the directory
			_mutex.lock();

  		DIR *dp;
		  dp = opendir(_directory.c_str());
		  if (dp == NULL) {
		    LOG_ERR("Error opening directory '%s': %s", _directory.c_str(), strerror(errno));
		    return;
		  }

			// iterate all entries and see what changed
			struct dirent* entry;
			set<string> currEntries;
			while((entry = readdir(dp))) {
				// are we interested in such a file?
				if (!filter(entry->d_name)) {
					continue;
				}
				currEntries.insert(entry->d_name);
				
				// see if the file was changed
				char* filename;
				asprintf(&filename, "%s/%s", _directory.c_str(), entry->d_name);

				struct stat fileStat;
				if (stat(filename, &fileStat) != 0) {
   				LOG_ERR("Error with stat on directory entry '%s': %s", filename, strerror(errno));
					free(filename);
					continue;
				}

				if (fileStat.st_mode & S_IFDIR) {
					// ignore directories
					free(filename);
					continue;
				}

				if (_knownEntries.find(entry->d_name) != _knownEntries.end()) {
					// we have seen this entry before
					struct stat oldStat = _knownEntries[entry->d_name];
					if (oldStat.st_mtime < fileStat.st_mtime) {
						publishModifiedFile(entry->d_name, fileStat);
					}
				} else {
					// we have not yet seen this entry
					publishNewFile(entry->d_name, fileStat);
				}

				free(filename);
				_knownEntries[entry->d_name] = fileStat; // gets copied on insertion
			}
			closedir(dp);

			// are there any known entries we have not seen this time around?
			map<string, struct stat>::iterator fileIter = _knownEntries.begin();
			while(fileIter != _knownEntries.end()) {
				if (currEntries.find(fileIter->first) == currEntries.end()) {
					// we used to know this file
					publishRemovedFile(fileIter->first);
					_knownEntries.erase(fileIter->first);
				}
				fileIter++;
			}
			// remember when we last checked the directory for modifications
			time(&_lastChecked);
			_mutex.unlock();
		}
		Thread::sleepMs(500);
	}
}

void DirectoryMonitor::publishNewFile(const string& fileName, struct stat fileStat) {
	LOG_DEBUG("New file %s found", fileName.c_str());

	int size = fileStat.st_size;
	char* buffer = readFileIntoBuffer(fileName, size);
	if (buffer != NULL) {
		Message* msg = new Message();
		msg->setMeta("file", fileName);
		msg->setMeta("directory", _directory);
		msg->setMeta("operation", "added");
		msg->setData(buffer, size);

		LOG_DEBUG("Publishing %d bytes from file %s", size, fileName.c_str());

		_pub->send(msg);
		delete msg;
	}
}

void DirectoryMonitor::publishModifiedFile(const string& fileName, struct stat fileStat) {
	LOG_DEBUG("Modified file %s", fileName.c_str());
	
	int size = fileStat.st_size;
	char* buffer = readFileIntoBuffer(fileName, size);
	if (buffer != NULL) {
		Message* msg = new Message();
		msg->setMeta("file", fileName);
		msg->setMeta("directory", _directory);
		msg->setMeta("operation", "modified");
		msg->setData(buffer, size);

		LOG_DEBUG("Publishing %d bytes from file %s", size, fileName.c_str());

		_pub->send(msg);
		delete msg;
	}
}

void DirectoryMonitor::publishRemovedFile(const string& fileName) {
	LOG_DEBUG("Removed file %s", fileName.c_str());	
	
	Message* msg = new Message();
	msg->setMeta("file", fileName);
	msg->setMeta("directory", _directory);
	msg->setMeta("operation", "removed");
	_pub->send(msg);
	delete msg;
}

char* DirectoryMonitor::readFileIntoBuffer(const string& fileName, int size) {

	char* absFilename;
	asprintf(&absFilename, "%s/%s", _directory.c_str(), fileName.c_str());
	char* buffer = (char*)malloc(size);

	FILE* fd;
	if ((fd = fopen(absFilename, "r")) == NULL) {
		LOG_ERR("Could not open %s: %s", absFilename, strerror(errno));
		free(buffer);
		free(absFilename);
		return NULL;
	}

	int n;
	if (size > 0) {
		if ((n = fread(buffer, 1, size, fd)) != size) {
			LOG_ERR("Expected to read %d bytes form %s, but got %d", size, absFilename, n);
			free(buffer);
			free(absFilename);
			return NULL;
		}
	}	

	fclose(fd);
	free(absFilename);
	return buffer;
}

}
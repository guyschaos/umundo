#include "umundo/util/DirectoryMonitor.h"

#ifndef WIN32
#include <dirent.h>
#else
#include <strsafe.h>
#endif

#include <stdio.h>
#include <string.h>

namespace umundo {

DirectoryMonitor::DirectoryMonitor(string directory, string channelName) :
	_directory(directory), _channelName(channelName), _lastChecked(0) {
	_greeter = new DirectoryMonitor::DirMonGreeter(this);
	_resender = new DirectoryMonitor::DirMonResender(this);
  // only connect if we have a channelname set
  if (_channelName.length() > 0) {
    _sub = new Subscriber(_channelName, _resender);
    _pub = new Publisher(_channelName);
    _pub->setGreeter(_greeter);
  }
}

set<Publisher*> DirectoryMonitor::getPublishers() {
	set<Publisher*> pubs;
	if (_channelName.length() > 0)
		pubs.insert(_pub);
	return pubs;
}

set<Subscriber*> DirectoryMonitor::getSubscribers() {
	set<Subscriber*> subs;
	if (_channelName.length() > 0)
		subs.insert(_sub);
	return subs;
}

DirectoryMonitor::~DirectoryMonitor() {
	delete _pub;
	delete _sub;
	delete _greeter;
	delete _resender;
}

void DirectoryMonitor::DirMonResender::receive(Message* msg) {
	// does this resend request concern our directory?
	if (msg->getMeta().find("directory") == msg->getMeta().end() || msg->getMeta("directory").compare(_monitor->_directory) != 0)
		return;
	// is this actually a resend request?
	if (msg->getMeta().find("operation") == msg->getMeta().end() || msg->getMeta("operation").compare("resend") != 0)
		return;
	// was a filename given?
	if (msg->getMeta().find("file") == msg->getMeta().end())
		return;
	// did we publish info on such a file before?
	if (_monitor->_knownEntries.find(msg->getMeta("file")) == _monitor->_knownEntries.end())
		return;

	// evrything worked out fine, we are requested to resend data
	_monitor->_mutex.lock();

	string filename = msg->getMeta("file");
	struct stat fStat = _monitor->_knownEntries[msg->getMeta("file")];
	char* buffer = _monitor->readFileIntoBuffer(filename, fStat.st_size);
	if (buffer != NULL) {
		Message* reply;
		if (msg->getMeta().find("replyTo") != msg->getMeta().end()) {
			reply = Message::toSubscriber(msg->getMeta("replyTo"));
		} else {
			reply = new Message();
		}

		reply->setMeta("file", msg->getMeta("file"));
		reply->setMeta("directory", _monitor->_directory);
		reply->setMeta("operation", "added");
		reply->setData(buffer, fStat.st_size);
		_monitor->_pub->send(reply);
		delete reply;
	}

	_monitor->_mutex.unlock();
	return;
}

void DirectoryMonitor::DirMonGreeter::welcome(Publisher* pub, const string nodeId, const string subId) {
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

		if (_channelName.length() > 0) {
			while(_pub->waitForSubscribers(0) == 0) {
				LOG_DEBUG("Not publishing as none is listening");
				_pub->waitForSubscribers(1);
			}
		}

		if ((unsigned)dirStat.st_mtime >= (unsigned)_lastChecked) {
			// there are changes in the directory
			_mutex.lock();
			set<string> currEntries;
#ifndef WIN32
			DIR *dp;
			dp = opendir(_directory.c_str());
			if (dp == NULL) {
				LOG_ERR("Error opening directory '%s': %s", _directory.c_str(), strerror(errno));
				return;
			}
			// iterate all entries and see what changed
			struct dirent* entry;
			while((entry = readdir(dp))) {
				string dname = entry->d_name;
#else
			WIN32_FIND_DATA ffd;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			TCHAR szDir[MAX_PATH];
			StringCchCopy(szDir, MAX_PATH, _directory.c_str());
			StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

			hFind = FindFirstFile(szDir, &ffd);
			do {
				string dname = ffd.cFileName;
#endif

				// are we interested in such a file?
				if (!filter(dname)) {
					continue;
				}
				currEntries.insert(dname);

				// see if the file was changed
				char* filename;
				asprintf(&filename, "%s/%s", _directory.c_str(), dname.c_str());

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

				if (_knownEntries.find(dname) != _knownEntries.end()) {
					// we have seen this entry before
					struct stat oldStat = _knownEntries[dname];
					if (oldStat.st_mtime < fileStat.st_mtime) {
						publishModifiedFile(dname, fileStat);
					}
				} else {
					// we have not yet seen this entry
					publishNewFile(dname, fileStat);
				}

				free(filename);
				_knownEntries[dname] = fileStat; // gets copied on insertion
#ifndef WIN32
			}
			closedir(dp);
#else
			} while (FindNextFile(hFind, &ffd) != 0);
			FindClose(hFind);
#endif
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
#ifndef WIN32
			time(&_lastChecked);
#else
			// TODO: this will fail with subsecond updates to the directory
			_lastChecked = dirStat.st_mtime + 1;
#endif
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
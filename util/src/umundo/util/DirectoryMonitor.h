#ifndef DIRECTORYMONITOR_H_XVF5N5JI
#define DIRECTORYMONITOR_H_XVF5N5JI

#include <sys/stat.h>

#include "umundo/connection/Node.h"
#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"
#include "umundo/common/Message.h"
#include "umundo/connection/Publisher.h"

namespace umundo {

class DirectoryMonitor : public Thread, public Connectable {
public:
	DirectoryMonitor(string directory, string channelName);
	virtual ~DirectoryMonitor() {};
	
	virtual bool filter(const string& filename) { return true; }
	
	virtual void publishNewFile(const string&, struct stat);
	virtual void publishModifiedFile(const string&, struct stat);
	virtual void publishRemovedFile(const string&);
	
	set<Publisher*> getPublishers();

	void run();

protected:
	
	/**
	 * Greet new subscribers with a full list of files
	 */
	class DirectoryMonitorGreeter : public Greeter {
	public:
		DirectoryMonitorGreeter(DirectoryMonitor* monitor) : _monitor(monitor) {}
		void welcome(Publisher* pub, const string nodeId, const string subId);
		
		DirectoryMonitor* _monitor;
	};
	
	char* readFileIntoBuffer(const string&, int);

	Publisher* _pub;
	string _directory;
	string _channelName;
	time_t _lastChecked;
	Mutex _mutex;
	map<string, struct stat> _knownEntries;
};

}

#endif /* end of include guard: DIRECTORYMONITOR_H_XVF5N5JI */

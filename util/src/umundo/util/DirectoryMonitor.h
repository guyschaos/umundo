#ifndef DIRECTORYMONITOR_H_XVF5N5JI
#define DIRECTORYMONITOR_H_XVF5N5JI

#include <sys/stat.h>

#include "umundo/connection/Node.h"
#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"
#include "umundo/common/Message.h"
#include "umundo/connection/Publisher.h"
#include "umundo/connection/Subscriber.h"

namespace umundo {

/**
 * Monitor a given directory and publish all files matching the optional filter.
 */
class DirectoryMonitor : public Thread, public Connectable {
public:
	DirectoryMonitor(string directory, string channelName = string());
	virtual ~DirectoryMonitor();
	
	virtual bool filter(const string& filename) { return true; }
	
	virtual void publishNewFile(const string&, struct stat);
	virtual void publishModifiedFile(const string&, struct stat);
	virtual void publishRemovedFile(const string&);
	
	set<Publisher*> getPublishers();
	set<Subscriber*> getSubscribers();

	void run();

protected:
	
	/**
	 * Greet new subscribers with a full list of files
	 */
	class DirMonGreeter : public Greeter {
	public:
		DirMonGreeter(DirectoryMonitor* monitor) : _monitor(monitor) {}
		void welcome(Publisher* pub, const string nodeId, const string subId);
		
		DirectoryMonitor* _monitor;
	};
	
	/**
	 *
	 */
	class DirMonResender : public Receiver {
	public:
		DirMonResender(DirectoryMonitor* monitor) : _monitor(monitor) {}
		void receive(Message* msg);
		
		DirectoryMonitor* _monitor;
	};
	
	char* readFileIntoBuffer(const string&, int);

	Publisher* _pub;
	Subscriber* _sub;
	
	DirMonGreeter* _greeter;
	DirMonResender* _resender;
	
	string _directory;
	string _channelName;
	time_t _lastChecked;
	Mutex _mutex;
	map<string, struct stat> _knownEntries;
};

}

#endif /* end of include guard: DIRECTORYMONITOR_H_XVF5N5JI */

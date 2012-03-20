#ifndef SERVICEMANAGER_H_D2RP19OS
#define SERVICEMANAGER_H_D2RP19OS

#include <umundo/core.h>
#include <umundo/s11n.h>
#include "umundo/rpc/Service.h"

namespace umundo {

class ServiceManager : public Receiver {
public:
	ServiceManager(Node* node);
	virtual ~ServiceManager();

	void registerService(Service*);
	void removeService(Service*);
	Service* getPrototype(const string&);
	Node* getNode() {
		return _node;
	}

	void receive(Message* msg);

	const string find(const string&);

	map<string, Monitor> _findRequests;
	map<string, string> _findResponses;
	map<string, Service*> _services;

	Node* _node;
	Publisher* _svcPub;   ///< publish service queries
	Subscriber* _svcSub;  ///< subscribe to service queries
	Publisher* _reqPub;   ///<
	Subscriber* _respSub;
};

}

#endif /* end of include guard: SERVICEMANAGER_H_D2RP19OS */

#ifndef SERVICEMANAGER_H_D2RP19OS
#define SERVICEMANAGER_H_D2RP19OS

#include <umundo/core.h>
#include <umundo/s11n.h>
#include "umundo/rpc/Service.h"

namespace umundo {

class ServiceManager : public Receiver, public Connectable {
public:
	ServiceManager();
	virtual ~ServiceManager();

	void addService(Service*);
	void addService(Service*, ServiceDescription*);
	void removeService(Service*);

  // Connectable interface
  std::set<umundo::Publisher*> getPublishers();
	std::set<umundo::Subscriber*> getSubscribers();
  void addedToNode(Node* node);
  void removedFromNode(Node* node);

	void receive(Message* msg);

  ServiceDescription* find(ServiceFilter*);
	void startQuery(ServiceFilter*, ResultSet<ServiceDescription>*);
	void stopQuery(ServiceFilter*);

	map<string, Monitor> _findRequests;
	map<string, Message*> _findResponses;
	map<intptr_t, Service*> _svc;
	map<intptr_t, ServiceDescription*> _svcDesc;
	map<ServiceFilter*, ResultSet<ServiceDescription>*> _svcQueries;

	std::set<Node*> _nodes;
	Publisher* _svcPub;   ///< publish service queries
	Subscriber* _svcSub;  ///< subscribe to service queries
  Mutex _mutex;
};

}

#endif /* end of include guard: SERVICEMANAGER_H_D2RP19OS */

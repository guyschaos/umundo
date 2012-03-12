#ifndef SERVICE_H_8J9Z1YLY
#define SERVICE_H_8J9Z1YLY

#include <umundo/core.h>
#include <umundo/s11n.h>

namespace umundo {

class ServiceManager;

class ServiceStub : public TypedReceiver {
public:
  ServiceStub(ServiceManager* svcMgr, const string& channel);
	virtual ~ServiceStub();
	virtual const string& getName();
	virtual const string& getChannelName();

	virtual void receive(void* object, Message* msg);

protected:
	ServiceStub() {};
	
	void addToNode(Node*);
	void removeFromNode(Node*);
	
	void callStubMethod(const string&, void*, const string&, void*&, const string&);

	string _channelName;
	string _serviceName;	
	TypedPublisher* _rpcPub;
	TypedSubscriber* _rpcSub;
	
	map<string, Monitor> _requests;
	map<string, void*> _responses;
	
	Mutex _mutex;
	
	friend class ServiceManager;
};

class Service : public ServiceStub {
public:
	Service();
	virtual ~Service();

	virtual void receive(void* object, Message* msg);

protected:	
	virtual void callMethod(string&, void*, const string&, void*&, const string&) = 0;

};

}


#endif /* end of include guard: SERVICE_H_8J9Z1YLY */

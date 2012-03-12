#include "umundo/rpc/Service.h"
#include "umundo/rpc/ServiceManager.h"

namespace umundo {
	
ServiceStub::ServiceStub(ServiceManager* svcMgr, const string& serviceName) {
  _channelName = svcMgr->find(serviceName);
	_rpcPub = new TypedPublisher(_channelName);
	_rpcSub = new TypedSubscriber(_channelName, this);
  addToNode(svcMgr->getNode());
  _rpcPub->waitForSubscribers(1);
}

ServiceStub::~ServiceStub() {
	
}

const string& ServiceStub::getChannelName() {
	return _channelName;
}

const string& ServiceStub::getName() {
	return _serviceName;
}

void ServiceStub::addToNode(Node* node) {
	node->addSubscriber(_rpcSub);
	node->addPublisher(_rpcPub);
}

void ServiceStub::removeFromNode(Node* node) {
	node->removeSubscriber(_rpcSub);
	node->removePublisher(_rpcPub);
}

void ServiceStub::callStubMethod(const string& name, void* in, const string& inType, void* &out, const string& outType) {
	Message* rpcReqMsg = _rpcPub->prepareMsg(inType, in);
	string reqId = UUID::getUUID();
	rpcReqMsg->setMeta("reqId", reqId);
	rpcReqMsg->setMeta("methodName", name);
	rpcReqMsg->setMeta("outType", outType);
	assert(_requests.find(reqId) == _requests.end());
	_requests[reqId] = Monitor();
	_rpcPub->send(rpcReqMsg);
	_requests[reqId].wait();
	_requests.erase(reqId);
  out = _responses[reqId];
  _responses.erase(reqId);
	delete rpcReqMsg;
}


void ServiceStub::receive(void* obj, Message* msg) {
  if (msg->getMeta().find("respId") != msg->getMeta().end()) {
		string respId = msg->getMeta("respId");
		if (_requests.find(respId) != _requests.end()) {
      _responses[respId] = obj;
      _requests[respId].signal();
		}
	}
}


Service::Service() {
	_channelName = UUID::getUUID();
  _rpcPub = new TypedPublisher(_channelName);
	_rpcSub = new TypedSubscriber(_channelName, this);
}

Service::~Service() {
	
}

void Service::receive(void* obj, Message* msg) {
	// somone wants a method called
	if (msg->getMeta().find("methodName") != msg->getMeta().end()) {
		string methodName = msg->getMeta("methodName");
		string inType = msg->getMeta("type");
		string outType = msg->getMeta("outType");
		void* out = NULL;
		callMethod(methodName, obj, inType, out, outType);
		Message* rpcReplMsg = _rpcPub->prepareMsg(outType, out);
    rpcReplMsg->setMeta("respId", msg->getMeta("reqId"));
		_rpcPub->send(rpcReplMsg);
		delete rpcReplMsg;
	}
}

}
#include "umundo/rpc/ServiceManager.h"

namespace umundo {

ServiceManager::ServiceManager(Node* node) {
	_node = node;
	_svcPub = new Publisher("umundo.sd");
	_svcSub = new Subscriber("umundo.sd", this);
	_node->addSubscriber(_svcSub);
	_node->addPublisher(_svcPub);
}

ServiceManager::~ServiceManager() {
}

const string ServiceManager::find(const string& serviceName) {
	Message* findMsg = new Message();
	string reqId = UUID::getUUID();
	findMsg->setMeta("type", "serviceDisc");
	findMsg->setMeta("serviceName", serviceName);
	findMsg->setMeta("reqId", reqId.c_str());
  _svcPub->waitForSubscribers(1);
	_svcPub->send(findMsg);
	
	_findRequests[reqId] = Monitor();
	_findRequests[reqId].wait();
	delete findMsg;
	if (_findResponses.find(reqId) != _findResponses.end()) {
		string channelName = _findResponses[reqId];
		_findResponses.erase(reqId);
		return channelName;
	}
	return "";
}

void ServiceManager::receive(Message* msg) {
	// is this a response for one of our requests?
	if (msg->getMeta().find("respId") != msg->getMeta().end()) {
		string respId = msg->getMeta("respId");
		if (_findRequests.find(respId) != _findRequests.end()) {
			_findResponses[respId] = msg->getMeta("channelName");
			_findRequests[respId].signal();
			_findRequests.erase(respId);
		}
	}

	// is someone asking for a service?
	if (msg->getMeta().find("type") != msg->getMeta().end() && 
			msg->getMeta("type").compare("serviceDisc") == 0) {
		string serviceName = msg->getMeta("serviceName");
		if (_services.find(serviceName) != _services.end()) {
			// we do have such a service
			Message* foundMsg = new Message();
			foundMsg->setMeta("respId", msg->getMeta("reqId"));
			foundMsg->setMeta("channelName", _services[serviceName]->getChannelName());
			_svcPub->send(foundMsg);
			delete foundMsg;
		}
	}
}

Service* ServiceManager::getPrototype(const string& serviceName) {
	if (_services.find(serviceName) != _services.end()) {
		return _services[serviceName];
	}
	return NULL;
}

void ServiceManager::registerService(Service* service) {
	string serviceName = service->getName();
	if (_services.find(serviceName) != _services.end()) {
		_services[serviceName]->removeFromNode(_node);
		delete _services[serviceName];
		_services.erase(serviceName);
	}
	_services[serviceName] = service;
	service->addToNode(_node);
}

void ServiceManager::removeService(Service* service) {
	string serviceName = service->getName();
	if (_services.find(serviceName) != _services.end()) {
		delete _services[serviceName];
		_services.erase(serviceName);
	}
}

}
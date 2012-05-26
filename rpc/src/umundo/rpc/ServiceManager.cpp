#include "umundo/rpc/ServiceManager.h"

namespace umundo {

ServiceManager::ServiceManager() {
	_svcPub = new Publisher("umundo.sd");
	_svcSub = new Subscriber("umundo.sd", this);
}

ServiceManager::~ServiceManager() {
  delete _svcPub;
  delete _svcSub;
}

std::set<umundo::Publisher*> ServiceManager::getPublishers() {
  set<Publisher*> pubs;
  pubs.insert(_svcPub);
  return pubs;
}
std::set<umundo::Subscriber*> ServiceManager::getSubscribers() {
  set<Subscriber*> subs;
  subs.insert(_svcSub);
  return subs;
}

void ServiceManager::addedToNode(Node* node) {
  ScopeLock lock(&_mutex);
	map<intptr_t, Service*>::iterator svcIter = _svc.begin();
  while(svcIter != _svc.end()) {
    node->connect(svcIter->second);
    svcIter++;
  }
  _nodes.insert(node);
}

void ServiceManager::removedFromNode(Node* node) {
  ScopeLock lock(&_mutex);
  if (_nodes.find(node) == _nodes.end())
    return;

	map<intptr_t, Service*>::iterator svcIter = _svc.begin();
  while(svcIter != _svc.end()) {
    node->disconnect(svcIter->second);
    svcIter++;
  }
  _nodes.erase(node);
}

ServiceDescription* ServiceManager::find(ServiceFilter* svcFilter) {
	Message* findMsg = svcFilter->toMessage();
	string reqId = UUID::getUUID();
	findMsg->setMeta("type", "serviceDisc");
	findMsg->setMeta("reqId", reqId.c_str());
	_svcPub->waitForSubscribers(1);
	_svcPub->send(findMsg);
	delete findMsg;

	_findRequests[reqId] = Monitor();
  _findRequests[reqId].wait();

	if (_findResponses.find(reqId) != _findResponses.end()) {
		Message* foundMsg = _findResponses[reqId];
    assert(foundMsg != NULL);
    ServiceDescription* svcDesc = new ServiceDescription(foundMsg);
    svcDesc->_svcManager = this;
		_findResponses.erase(reqId);
    delete foundMsg;
		return svcDesc;
	}
	return NULL;
}

void ServiceManager::receive(Message* msg) {
  ScopeLock lock(&_mutex);
	// is this a response for one of our requests?
	if (msg->getMeta().find("respId") != msg->getMeta().end()) {
		string respId = msg->getMeta("respId");
		if (_findRequests.find(respId) != _findRequests.end()) {
			_findResponses[respId] = new Message(*msg);
			_findRequests[respId].signal();
			_findRequests.erase(respId);
		}
	}

	// is someone asking for a service?
	if (msg->getMeta().find("type") != msg->getMeta().end() &&
	        msg->getMeta("type").compare("serviceDisc") == 0) {
    ServiceFilter* filter = new ServiceFilter(msg);

    map<intptr_t, ServiceDescription*>::iterator svcDescIter = _svcDesc.begin();
    while(svcDescIter != _svcDesc.end()) {
      if (filter->matches(svcDescIter->second)) {
        Message* foundMsg = svcDescIter->second->toMessage();
        foundMsg->setMeta("respId", msg->getMeta("reqId"));
        foundMsg->setMeta("desc:channel", _svc[svcDescIter->first]->getChannelName());
        _svcPub->send(foundMsg);
        delete foundMsg;
      }
      svcDescIter++;
    }
	}
}

void ServiceManager::addService(Service* service) {
  addService(service, new ServiceDescription(service->getName(), map<string, string>()));
}
  
void ServiceManager::addService(Service* service, ServiceDescription* desc) {
  ScopeLock lock(&_mutex);

  intptr_t svcPtr = (intptr_t)service;
  _svc[svcPtr] = service;
  _svcDesc[svcPtr] = desc;
  
  std::set<Node*>::iterator nodeIter;
  nodeIter = _nodes.begin();
  while(nodeIter != _nodes.end()) {
    (*nodeIter++)->connect(service);
  }
}

void ServiceManager::removeService(Service* service) {
  ScopeLock lock(&_mutex);

  intptr_t svcPtr = (intptr_t)service;
	if (_svc.find(svcPtr) != _svc.end()) {
    std::set<Node*>::iterator nodeIter = _nodes.begin();
    while(nodeIter != _nodes.end()) {
      (*nodeIter++)->disconnect(service);
    }
		_svc.erase(svcPtr);
		_svcDesc.erase(svcPtr);
	}
}

}
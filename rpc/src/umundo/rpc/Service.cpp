#include "umundo/rpc/Service.h"
#include "umundo/rpc/ServiceManager.h"
#include <stdlib.h>

namespace umundo {

ServiceFilter::ServiceFilter(const string& svcName) {
  _svcName = svcName;
}

ServiceFilter::ServiceFilter(Message* msg) {
  _svcName = msg->getMeta("serviceName");
  map<string, string> meta = msg->getMeta();
  map<string, string>::const_iterator metaIter = meta.begin();
  while(metaIter != meta.end()) {
    string key = metaIter->first;
    string value = metaIter->second;
    if (key.length() > 5 && key.compare(0, 5, "cond:") == 0) {
      key = key.substr(5, key.length());
      _condition[key] = value;
      if (meta.find("pred:" + key) != meta.end()) {
        _predicate[key] = (Predicate)atoi(meta["pred:" + key].c_str());
      }
    }
    metaIter++;
  }
}

Message* ServiceFilter::toMessage() {
  Message* msg = new Message();
  msg->setMeta("serviceName", _svcName);
  map<string, string>::iterator condIter = _condition.begin();
  while(condIter != _condition.end()) {
    msg->setMeta("cond:" + condIter->first, condIter->second);
    condIter++;
  }
  map<string, Predicate>::iterator predIter = _predicate.begin();
  while(predIter != _predicate.end()) {
    std::stringstream ss;
    ss << predIter->second;
    msg->setMeta("pred:" + predIter->first, ss.str());
    predIter++;
  }
  return msg;
}

void ServiceFilter::addRule(const string& key, const string& pattern, Predicate pred) {
  _condition[key] = pattern;
  _predicate[key] = pred;
}

bool ServiceFilter::matches(ServiceDescription* svcDesc) {
  // service name has to be the same
  if (_svcName.compare(svcDesc->getName()) != 0)
    return false;
  
  // check filter
  map<string, string>::iterator condIter = _condition.begin();
  while(condIter != _condition.end()) {
    string key = condIter->first;
    string pattern = condIter->second;
    string value = svcDesc->getProperty(key);
    Predicate pred = OP_EQUALS;
    if (_predicate.find(key) != _predicate.end())
      pred = _predicate[key];

    switch (pred) {
      case OP_EQUALS:
        if (pattern.compare(value) != 0) {
          return false;
        }
        break;
        
      default:
        break;
    }
    
    condIter++;
  }
  return true;
}

ServiceDescription::ServiceDescription(const string& svcName, map<string, string> properties) {
  _svcName = svcName;
  _properties = properties;
}

ServiceDescription::ServiceDescription(Message* msg) {
  _svcName = msg->getMeta("desc:name");
  _channelName = msg->getMeta("desc:channel");
  map<string, string>::const_iterator metaIter = msg->getMeta().begin();
  while(metaIter != msg->getMeta().end()) {
    string key = metaIter->first;
    string value = metaIter->second;
    if (key.length() > 5 && key.compare(0, 5, "desc:") == 0) {
      key = key.substr(5, key.length());
      _properties[key] = value;
    }
    metaIter++;
  }
}

Message* ServiceDescription::toMessage() { 
  Message* msg = new Message();
  map<string, string>::const_iterator propIter = _properties.begin();
  while(propIter != _properties.end()) {
    msg->setMeta("desc:" + propIter->first, propIter->second);
    propIter++;
  }
  msg->setMeta("desc:name", _svcName);
  msg->setMeta("desc:channel", _channelName);
  return msg;
}

const string ServiceDescription::getName() { 
  return _svcName; 
}
  
const string ServiceDescription::getChannelName() { 
  return _channelName; 
}
  
const map<string, string>& ServiceDescription::getProperties() { 
  return _properties; 
}

const string ServiceDescription::getProperty(const string& key) { 
  return _properties[key]; 
}

void ServiceDescription::setProperty(const string& key, const string& value) { 
  _properties[key] = value; 
}

ServiceStub::ServiceStub(ServiceDescription* svcDesc) {
  _channelName = svcDesc->getChannelName();
	_rpcPub = new TypedPublisher(_channelName);
	_rpcSub = new TypedSubscriber(_channelName, this);

  // TODO: this is a hack ..
  set<Node*>::iterator nodeIter = svcDesc->_svcManager->_nodes.begin();
  while(nodeIter != svcDesc->_svcManager->_nodes.end()) {
    (*nodeIter)->addSubscriber(_rpcSub);
    (*nodeIter)->addPublisher(_rpcPub);
    nodeIter++;
  }
	_rpcPub->waitForSubscribers(1);
}

ServiceStub::ServiceStub(const string& channelName) {
  _channelName = channelName;
	_rpcPub = new TypedPublisher(_channelName);
	_rpcSub = new TypedSubscriber(_channelName, this);
//	_rpcPub->waitForSubscribers(1);
}

ServiceStub::~ServiceStub() {
  delete _rpcPub;
  delete _rpcSub;
}

std::set<umundo::Publisher*> ServiceStub::getPublishers() {
  set<Publisher*> pubs;
  pubs.insert(_rpcPub);
  return pubs;
}
std::set<umundo::Subscriber*> ServiceStub::getSubscribers() {
  set<Subscriber*> subs;
  subs.insert(_rpcSub);
  return subs;
}

const string& ServiceStub::getChannelName() {
	return _channelName;
}

const string& ServiceStub::getName() {
	return _serviceName;
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
	UMUNDO_WAIT(_requests[reqId]);
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
			UMUNDO_SIGNAL(_requests[respId]);
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
		cleanUpObjects(methodName, obj, out);
		delete rpcReplMsg;
	}
}

}
#include "umundo/common/Node.h"
#include "umundo/discovery/NodeQuery.h"
#include "umundo/common/Factory.h"

namespace umundo {

NodeQuery::NodeQuery(string domain, ResultSet<NodeStub>* listener) :
	_notifyImmediately(true), _domain(domain), _transport("tcp"), _listener(listener) {
}

NodeQuery::~NodeQuery() {
}

void NodeQuery::found(shared_ptr<NodeStub> node) {
	_mutex.lock();
	if (_notifyImmediately) {
		if (_nodes.find(node->getUUID()) != _nodes.end()) {
//      LOG_DEBUG("Changed node %s", SHORT_UUID(node->getUUID()).c_str());
			_listener->changed(node);
		} else {
      LOG_DEBUG("Added node %s", SHORT_UUID(node->getUUID()).c_str());
			_listener->added(node);
      _nodes[node->getUUID()] = node;
		}
	} else {
		_pendingFinds.insert(node);
	}
	_mutex.unlock();
}

void NodeQuery::removed(shared_ptr<NodeStub> node) {
	_mutex.lock();
	if (_notifyImmediately) {
    LOG_DEBUG("Removed node %s", SHORT_UUID(node->getUUID()).c_str());
		_listener->removed(node);
    _nodes.erase(node->getUUID());
	} else {
		_pendingRemovals.insert(node);
	}
	_mutex.unlock();
}

void NodeQuery::notifyImmediately(bool notifyImmediately) {
	_notifyImmediately = notifyImmediately;
}

void NodeQuery::notifyResultSet() {
	_mutex.lock();
	set<shared_ptr<NodeStub> >::const_iterator nodeIter;

	for (nodeIter = _pendingRemovals.begin(); nodeIter != _pendingRemovals.end(); nodeIter++) {
    LOG_DEBUG("Removed node %s", SHORT_UUID((*nodeIter)->getUUID()).c_str());
		_listener->removed(*nodeIter);
		_nodes.erase((*nodeIter)->getUUID());
	}

	for (nodeIter = _pendingFinds.begin(); nodeIter != _pendingFinds.end(); nodeIter++) {
		if (_nodes.find((*nodeIter)->getUUID()) != _nodes.end()) {
//      LOG_DEBUG("Changed node %s", SHORT_UUID((*nodeIter)->getUUID()).c_str());
			_listener->changed(*nodeIter);			
		} else {
      LOG_DEBUG("Added node %s", SHORT_UUID((*nodeIter)->getUUID()).c_str());
			_listener->added(*nodeIter);
			_nodes[(*nodeIter)->getUUID()] = *nodeIter;
		}
	}

	_pendingRemovals.clear();
	_pendingFinds.clear();
	_mutex.unlock();
}

const string& NodeQuery::getDomain() {
	return _domain;
}

void NodeQuery::setTransport(string transport) {
	_transport = transport;
}

const string& NodeQuery::getTransport() {
	return _transport;
}

}
#include "umundo/discovery/NodeQuery.h"
#include "umundo/common/Factory.h"

namespace umundo {

NodeQuery::NodeQuery(string domain, ResultSet<NodeStub>* listener) :
	_notifyImmediately(true), _domain(domain), _transport("tcp"), _listener(listener) {
}

NodeQuery::~NodeQuery() {
}

void NodeQuery::added(shared_ptr<NodeStub> node) {
	_mutex.lock();
	if (_notifyImmediately) {
		_listener->added(node);
	} else {
		_pendingAdditions.insert(node);
	}
	_mutex.unlock();
}

void NodeQuery::changed(shared_ptr<NodeStub> node) {
	_mutex.lock();
	if (_notifyImmediately) {
		_listener->changed(node);
	} else {
		_pendingChanges.insert(node);
	}
	_mutex.unlock();
}

void NodeQuery::removed(shared_ptr<NodeStub> node) {
	_mutex.lock();
	if (_notifyImmediately) {
		_listener->removed(node);
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
	for (nodeIter = _pendingChanges.begin(); nodeIter != _pendingChanges.end(); nodeIter++) {
		_listener->changed(*nodeIter);
	}
	_pendingChanges.clear();

	for (nodeIter = _pendingRemovals.begin(); nodeIter != _pendingRemovals.end(); nodeIter++) {
		_listener->removed(*nodeIter);
	}
	_pendingRemovals.clear();

	for (nodeIter = _pendingAdditions.begin(); nodeIter != _pendingAdditions.end(); nodeIter++) {
		_listener->added(*nodeIter);
	}
	_pendingAdditions.clear();
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
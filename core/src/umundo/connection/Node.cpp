#include "umundo/connection/Node.h"

#include "umundo/common/Factory.h"
#include "umundo/common/UUID.h"
#include "umundo/discovery/Discovery.h"
#include "umundo/connection/Subscriber.h"
#include "umundo/connection/Publisher.h"

namespace umundo {

shared_ptr<Configuration> NodeConfig::create() {
	return shared_ptr<Configuration>(new NodeConfig());
}

NodeImpl::NodeImpl() {
	_uuid = UUID::getUUID();
}

int Node::instances = 0;
Node::Node() {
	_impl = boost::static_pointer_cast<NodeImpl>(Factory::create("node"));
	shared_ptr<Configuration> config = Factory::config("node");
	_impl->init(config);
	// add our node query
	Discovery::add(this);
	instances++;
}

Node::Node(string domain) {
	_impl = boost::static_pointer_cast<NodeImpl>(Factory::create("node"));
	shared_ptr<Configuration> config = Factory::config("node");
	_impl->setDomain(domain);
	_impl->init(config);
	Discovery::add(this);
	instances++;
}

Node::~Node() {
	Discovery::remove(this);
	instances--;
}

void Node::addSubscriber(Subscriber* sub) {
	_impl->addSubscriber(sub->_impl);
}

void Node::removeSubscriber(Subscriber* sub) {
	_impl->removeSubscriber(sub->_impl);
}

void Node::addPublisher(Publisher* pub) {
	_impl->addPublisher(pub->_impl);
}

void Node::removePublisher(Publisher* pub) {
	_impl->removePublisher(pub->_impl);
}

void Node::suspend() {
	_impl->suspend();
}

void Node::resume() {
	_impl->resume();
}

std::ostream& operator<<(std::ostream &out, const NodeStub* n) {
	out << SHORT_UUID(n->getUUID()) << ": ";
	out << n->getHost() << "";
	out << n->getDomain() << ":";
	out << n->getPort();

	// map<int, string>::const_iterator iFaceIter;
	// for (iFaceIter = n->_interfaces.begin(); iFaceIter != n->_interfaces.end(); iFaceIter++) {
	// 	out << "\t" << iFaceIter->first << ": " << iFaceIter->second << std::endl;
	// }
	return out;
}

}

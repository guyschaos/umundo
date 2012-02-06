#include "Node.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "common/Factory.h"
#include "discovery/Discovery.h"
#include "connection/Subscriber.h"
#include "connection/Publisher.h"

namespace umundo {

shared_ptr<Configuration> NodeConfig::create() {
  return shared_ptr<Configuration>(new NodeConfig());
}

NodeImpl::NodeImpl() {
	_uuid = boost::lexical_cast<string>(boost::uuids::random_generator()());
}

Node::Node() {
	_impl = boost::static_pointer_cast<NodeImpl>(Factory::create("node"));
	shared_ptr<NodeConfig> config = boost::static_pointer_cast<NodeConfig>(Factory::config("node"));
	_impl->init(config);
  // add our node query
	Discovery::add(this);
}

Node::Node(string domain) {
	_impl = boost::static_pointer_cast<NodeImpl>(Factory::create("node"));
	shared_ptr<NodeConfig> config = boost::static_pointer_cast<NodeConfig>(Factory::config("node"));
	config->domain = domain;
	_impl->init(config);
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

}
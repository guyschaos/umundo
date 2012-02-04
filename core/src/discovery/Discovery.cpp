#include "Discovery.h"
#include "connection/Publisher.h"
#include "common/Factory.h"

namespace umundo {


Discovery::Discovery() {
	_impl = boost::static_pointer_cast<DiscoveryImpl>(Factory::create("discovery"));
	shared_ptr<DiscoveryConfig> config = boost::static_pointer_cast<DiscoveryConfig>(Factory::config("discovery"));
	_impl->init(config);
}

Discovery::~Discovery() {
}

void Discovery::remove(Node* node) {
	boost::static_pointer_cast<DiscoveryImpl>(Factory::create("discovery"))->remove(node->_impl);
}

void Discovery::add(Node* node) {
	boost::static_pointer_cast<DiscoveryImpl>(Factory::create("discovery"))->add(node->_impl);
}

void Discovery::browse(shared_ptr<NodeQuery> discovery) {
	boost::static_pointer_cast<DiscoveryImpl>(Factory::create("discovery"))->browse(discovery);
}

void Discovery::unbrowse(shared_ptr<NodeQuery> discovery) {
	boost::static_pointer_cast<DiscoveryImpl>(Factory::create("discovery"))->unbrowse(discovery);
}

}
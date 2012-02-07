#include "umundo/discovery/Discovery.h"

#include "umundo/common/Factory.h"
#include "umundo/common/Node.h"

namespace umundo {

/**
 * @todo: This never gets called if we use the static functions - refactor
 */
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
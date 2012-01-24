#include "connection/Publisher.h"
#include "common/Factory.h"

namespace umundo {

Discovery::Discovery() {
	_impl = Factory::createDiscovery();
}

Discovery::~Discovery() {
}

void Discovery::remove(shared_ptr<Node> node) {
	Factory::createDiscovery()->remove(node);
}

void Discovery::add(shared_ptr<Node> node) {
	Factory::createDiscovery()->add(node);
}

void Discovery::browse(NodeQuery* discovery) {
	Factory::createDiscovery()->browse(discovery);
}

void Discovery::unbrowse(NodeQuery* discovery) {
	Factory::createDiscovery()->unbrowse(discovery);
}

}
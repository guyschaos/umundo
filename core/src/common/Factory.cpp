#include "common/Factory.h"

namespace umundo {

Factory* Factory::_instance = NULL;

Factory* Factory::getInstance() {
	if (Factory::_instance == NULL) {
		Factory::_instance = new Factory();
		assert(_instance->_prototypes["publisher"] != NULL);
		assert(_instance->_prototypes["subscriber"] != NULL);
		assert(_instance->_prototypes["discovery"] != NULL);
		assert(_instance->_prototypes["node"] != NULL);
	}
	return Factory::_instance;
}

Factory::Factory() {
	DEBUG_CTOR("Factory");
	_prototypes["publisher"] = new ZeroMQPublisher();
	_configures["publisher"] = new PublisherConfig();
	_prototypes["subscriber"] = new ZeroMQSubscriber();
	_configures["subscriber"] = new SubscriberConfig();
	_prototypes["node"] = new ZeroMQNode();
	_configures["node"] = new NodeConfig();
#ifdef DISC_BONJOUR
	_prototypes["discovery"] = new BonjourNodeDiscovery();
#endif
#ifdef DISC_AVAHI
	_prototypes["discovery"] = new AvahiNodeDiscovery();
#endif
}

shared_ptr<Configuration> Factory::config(string name) {
	return getInstance()->_configures[name]->create();
}

shared_ptr<Implementation> Factory::create(string name) {
	return getInstance()->_prototypes[name]->create();
}

}
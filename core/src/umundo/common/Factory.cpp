#include "umundo/common/Factory.h"

#include "umundo/connection/zeromq/ZeroMQNode.h"
#include "umundo/connection/zeromq/ZeroMQPublisher.h"
#include "umundo/connection/zeromq/ZeroMQSubscriber.h"
#include "umundo/config.h"

#ifdef DISC_BONJOUR
#include "umundo/discovery/bonjour/BonjourNodeDiscovery.h"
#endif

#ifdef DISC_AVAHI
#include "umundo/discovery/avahi/AvahiNodeDiscovery.h"
#endif

#if !(defined DISC_AVAHI || defined DISC_BONJOUR)
#error "No discovery implementation choosen"
#endif

#if !(defined NET_ZEROMQ)
#error "No discovery implementation choosen"
#endif

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
	Factory* factory = getInstance();
	if (factory->_configures.find(name) == factory->_configures.end()) {
		LOG_WARN("No configuration registered for %s", name.c_str());
		return shared_ptr<Configuration>();
	}
	return factory->_configures[name]->create();
}

shared_ptr<Implementation> Factory::create(string name) {
	Factory* factory = getInstance();
	if (factory->_prototypes.find(name) == factory->_prototypes.end()) {
		LOG_WARN("No prototype registered for %s", name.c_str());
		return shared_ptr<Implementation>();
	}
	return factory->_prototypes[name]->create();
}

void Factory::registerPrototype(string name, Implementation* prototype, Configuration* config) {
	Factory* factory = getInstance();
	if (factory->_prototypes.find(name) != factory->_prototypes.end()) {
		LOG_WARN("Overwriting existing prototype for %s", name.c_str());
	}
	factory->_prototypes[name] = prototype;
	factory->_configures[name] = config;
}

}
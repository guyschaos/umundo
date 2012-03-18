#include "umundo/common/Factory.h"
#include "umundo/common/UUID.h"

#include "umundo/connection/zeromq/ZeroMQNode.h"
#include "umundo/connection/zeromq/ZeroMQPublisher.h"
#include "umundo/connection/zeromq/ZeroMQSubscriber.h"
#include "umundo/config.h"

#include <algorithm>

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

string procUUID = UUID::getUUID();

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
	factory->_mutex.lock();
	if (factory->_configures.find(name) == factory->_configures.end()) {
		LOG_WARN("No configuration registered for %s", name.c_str());
		factory->_mutex.unlock();
		return shared_ptr<Configuration>();
	}
	shared_ptr<Configuration> config = factory->_configures[name]->create();
	factory->_mutex.unlock();
	return config;
}

shared_ptr<Implementation> Factory::create(string name) {
	Factory* factory = getInstance();
	factory->_mutex.lock();
	if (factory->_prototypes.find(name) == factory->_prototypes.end()) {
		LOG_WARN("No prototype registered for %s", name.c_str());
		factory->_mutex.unlock();
		return shared_ptr<Implementation>();
	}
	shared_ptr<Implementation> implementation = factory->_prototypes[name]->create();
//  weak_ptr<Implementation> weakImpl = implementation;
  if (implementation.get() != NULL && factory->_implementations.find(implementation) == factory->_implementations.end())
    factory->_implementations.insert(implementation);
	factory->_mutex.unlock();
	return implementation;
}

void Factory::registerPrototype(string name, Implementation* prototype, Configuration* config) {
	Factory* factory = getInstance();
	factory->_mutex.lock();
	if (factory->_prototypes.find(name) != factory->_prototypes.end()) {
		LOG_WARN("Overwriting existing prototype for %s", name.c_str());
	}
	factory->_prototypes[name] = prototype;
	factory->_configures[name] = config;
	factory->_mutex.unlock();
}

/**
 * Iterate created instances from newest to oldest and send suspend request.
 */
void Factory::suspendInstances() {
	Factory* factory = getInstance();
	factory->_mutex.lock();
	set<weak_ptr<Implementation> >::reverse_iterator implIter = factory->_implementations.rbegin();
	while(implIter != factory->_implementations.rend()) {
		shared_ptr<Implementation> implementation = implIter->lock();
    implIter++;
		if (implementation.get() != NULL) {
			implementation->suspend();
		} else {
			// I have not found a way to savely remove a reverse iterator - just rely on resumeInstances
		}
	}
	factory->_mutex.unlock();
}

/**
 * Iterate created instances from oldest to newest and send resume request.
 */
void Factory::resumeInstances() {
	Factory* factory = getInstance();
	factory->_mutex.lock();
	set<weak_ptr<Implementation> >::iterator implIter = factory->_implementations.begin();
	while(implIter != factory->_implementations.end()) {
		shared_ptr<Implementation> implementation = implIter->lock();
    implIter++;
		if (implementation.get() != NULL) {
			implementation->resume();
		} else {
			factory->_implementations.erase(implementation);
		}
	}
	factory->_mutex.unlock();
}

}
#include "common/Factory.h"

namespace umundo {

Factory* Factory::_instance = NULL;

Factory* Factory::getInstance() {
	if (Factory::_instance == NULL) {
		Factory::_instance = new Factory();
		assert(_instance->_publisherImpl != NULL);
		assert(_instance->_subscriberImpl != NULL);
		assert(_instance->_discoveryImpl != NULL);
	}
	return Factory::_instance;
}

Factory::Factory() {
	DEBUG_CTOR("Factory");
	_publisherImpl = new ZeroMQPublisher();
	_subscriberImpl = new ZeroMQSubscriber();
#ifdef DISC_BONJOUR
	_discoveryImpl = new BonjourNodeDiscovery();
#endif
#ifdef DISC_AVAHI
	_discoveryImpl = new AvahiNodeDiscovery();
#endif
}

PublisherImpl* Factory::createPublisher(std::string channelName) {
	return getInstance()->_publisherImpl->create(channelName);
}

SubscriberImpl* Factory::createSubscriber(std::string channelName, Receiver* receiver) {
	return getInstance()->_subscriberImpl->create(channelName, receiver);
}

DiscoveryImpl* Factory::createDiscovery() {
	return getInstance()->_discoveryImpl->create();
}

}
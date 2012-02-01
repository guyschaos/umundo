#ifndef FACTORY_H_BRZEE6H
#define FACTORY_H_BRZEE6H

#include "config.h"
#include "common/stdInc.h"

#include "common/NodeStub.h"
#include "common/ResultSet.h"
#include "connection/Publisher.h"
#include "connection/Subscriber.h"

#include "connection/zeromq/ZeroMQNode.h"
#include "connection/zeromq/ZeroMQPublisher.h"
#include "connection/zeromq/ZeroMQSubscriber.h"

#ifdef DISC_BONJOUR
#include "discovery/bonjour/BonjourNodeDiscovery.h"
#endif

#ifdef DISC_AVAHI
#include "discovery/avahi/AvahiNodeDiscovery.h"
#endif

namespace umundo {

/**
 * Creates instances of implementations for subsystems at runtime (factory pattern).
 *
 * This class realizes the Factory pattern by instantiating objects form prototypes. If you want to implement a specific subsystem
 * yourself, just inherit its base-class and register a prototype at the factory.
 *
 * \todo The *Impl classes need a destroy to delegate tidying up to the concrete implementors.
 *
 * \see PublisherImpl, SubscriberImpl, DiscoveryImpl
 */
class Factory {
public:
	static Factory* getInstance();
	static PublisherImpl* createPublisher(string);
	static SubscriberImpl* createSubscriber(string, Receiver*);
	static DiscoveryImpl* createDiscovery();

	PublisherImpl* _publisherImpl;
	SubscriberImpl* _subscriberImpl;
	DiscoveryImpl* _discoveryImpl;

protected:
	Factory();

private:
	static Factory* _instance;

};

}

#endif /* end of include guard: FACTORY_H_BRZEE6H */

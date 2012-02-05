#ifndef FACTORY_H_BRZEE6H
#define FACTORY_H_BRZEE6H

#include "config.h"
#include "common/stdInc.h"

#include "common/Implementation.h"
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
 * \see PublisherImpl, SubscriberImpl, DiscoveryImpl, NodeImpl
 */
class Factory {
public:
	static Factory* getInstance();
	static shared_ptr<Implementation> create(string);
	static shared_ptr<Configuration> config(string);
	static void registerPrototype(string, Implementation*, Configuration*);

protected:
	Factory();

private:
	map<string, Implementation*> _prototypes;
	map<string, Configuration*> _configures;
	static Factory* _instance;

};

}

#endif /* end of include guard: FACTORY_H_BRZEE6H */

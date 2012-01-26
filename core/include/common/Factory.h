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

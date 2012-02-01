#ifndef SUBSCRIBER_H_J64J09SP
#define SUBSCRIBER_H_J64J09SP

#include "common/stdInc.h"
#include "thread/Thread.h"

namespace umundo {

class SubscriberImpl;

/**
 * Interface for client classes to get byte-arrays from subscribers.
 */
class Receiver {
public:
	virtual ~Receiver() {}
	virtual void receive(char* buffer, size_t length) = 0;
	friend class Subscriber;
};

/**
* Subscriber abstraction (bridge pattern).
 */
class Subscriber {
public:

	Subscriber(string channelName, Receiver* receiver);
	virtual ~Subscriber();

	virtual const string& getChannelName();

protected:
	SubscriberImpl* _impl;
};

/**
 * Subscriber implementor basis class (bridge pattern).
 */
class SubscriberImpl : public Thread {
public:
	virtual SubscriberImpl* create(string channelName, Receiver* receiver) = 0;
	virtual const string& getChannelName() = 0;
};

}


#endif /* end of include guard: SUBSCRIBER_H_J64J09SP */

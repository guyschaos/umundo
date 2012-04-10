#ifndef SUBSCRIBER_H_J64J09SP
#define SUBSCRIBER_H_J64J09SP

#include "umundo/common/Common.h"
#include "umundo/common/Implementation.h"
#include "umundo/thread/Thread.h"

namespace umundo {

class Message;
class SubscriberImpl;

/**
 * Interface for client classes to get byte-arrays from subscribers.
 */
class Receiver {
public:
	virtual ~Receiver() {}
	virtual void receive(Message* msg) = 0;
	friend class Subscriber;
};

class SubscriberConfig : public Configuration {
public:
	virtual ~SubscriberConfig() {}
	shared_ptr<Configuration> create();
	string channelName;
	string uuid;
	Receiver* receiver;
};

/**
 * Subscriber implementor basis class (bridge pattern).
 */
class SubscriberImpl : public Thread, public Implementation {
public:
	virtual const string& getChannelName()           {
		return _channelName;
	}
	virtual void setChannelName(string channelName)  {
		_channelName = channelName;
	}
	virtual const Receiver* getReceiver()            {
		return _receiver;
	}
	virtual void setReceiver(Receiver* receiver)     {
		_receiver = receiver;
	}
	virtual const string& getUUID()                  {
		return _uuid;
	}
	virtual void setUUID(string uuid)                {
		_uuid = uuid;
	}

protected:
	Receiver* _receiver;
	string _channelName;
	string _uuid;
};

/**
 * Subscriber abstraction (bridge pattern).
 *
 * We need to overwrite everything to use the concrete implementors functions. The preferred
 * constructor is the Subscriber(string channelName, Receiver* receiver) one, the unqualified
 * constructor without a receiver and the setReceiver method are required for Java as we cannot
 * inherit publishers while being its receiver at the same time as is used for the TypedSubscriber.
 */
class Subscriber {
public:
	Subscriber(string channelName);
	Subscriber(string channelName, Receiver* receiver);
	virtual ~Subscriber();

	void setReceiver(Receiver* receiver);

	virtual const string& getChannelName()           {
		return _impl->getChannelName();
	}
	virtual void setChannelName(string channelName)  {
		_impl->setChannelName(channelName);
	}

protected:

	shared_ptr<SubscriberImpl> _impl;
	shared_ptr<SubscriberConfig> _config;
	friend class Node;
};

}


#endif /* end of include guard: SUBSCRIBER_H_J64J09SP */

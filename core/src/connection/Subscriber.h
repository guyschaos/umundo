#ifndef SUBSCRIBER_H_J64J09SP
#define SUBSCRIBER_H_J64J09SP

#include "common/stdInc.h"
#include "thread/Thread.h"
#include "common/Implementation.h"

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

class SubscriberConfig : public Configuration {
public:
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
	virtual const string& getChannelName()           { return _channelName; }
	virtual void setChannelName(string channelName)  { _channelName = channelName; }
  virtual const Receiver* getReceiver()            { return _receiver; }
  virtual void setReceiver(Receiver* receiver)     { _receiver = receiver; }
  virtual const string& getUUID()                  { return _uuid; }
	virtual void setUUID(string uuid)                { _uuid = uuid; }

protected:
  Receiver* _receiver;
  string _channelName;
  string _uuid;
};

/**
 * Subscriber abstraction (bridge pattern).
 *
 * We need to overwrite everything to use the concrete implementors functions
 */
class Subscriber {
public:
	Subscriber(string channelName, Receiver* receiver);
	virtual ~Subscriber();

	virtual const string& getChannelName()           { return _impl->getChannelName(); }
	virtual void setChannelName(string channelName)  { _impl->setChannelName(channelName); }

protected:
	shared_ptr<SubscriberImpl> _impl;
	shared_ptr<SubscriberConfig> _config;
	friend class Node;
};

}


#endif /* end of include guard: SUBSCRIBER_H_J64J09SP */

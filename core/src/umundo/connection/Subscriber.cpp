#include "umundo/connection/Subscriber.h"
#include "umundo/common/Factory.h"

namespace umundo {

shared_ptr<Configuration> SubscriberConfig::create() {
	return shared_ptr<SubscriberConfig>(new SubscriberConfig());
}

Subscriber::Subscriber(string channelName, Receiver* receiver) {
	_impl = boost::static_pointer_cast<SubscriberImpl>(Factory::create("subscriber", this));
	_config = boost::static_pointer_cast<SubscriberConfig>(Factory::config("subscriber"));
//	_config->channelName = channelName;
//	_config->receiver = receiver;
	_impl->setChannelName(channelName);
	_impl->setReceiver(receiver);
	_impl->init(_config);
}

Subscriber::~Subscriber() {
}

Subscriber::Subscriber(string channelName) {
	_impl = boost::static_pointer_cast<SubscriberImpl>(Factory::create("subscriber", this));
//	_config->channelName = channelName;
//	_config->receiver = receiver;
	_impl->setChannelName(channelName);
}

void Subscriber::setReceiver(Receiver* receiver) {
	_impl->setReceiver(receiver);
	_config = boost::static_pointer_cast<SubscriberConfig>(Factory::config("subscriber"));
	_impl->init(_config);	
}

}
#include "connection/Subscriber.h"
#include "common/Factory.h"

namespace umundo {

Subscriber::Subscriber(string channelName, Receiver* receiver) {
	_impl = Factory::createSubscriber(channelName, receiver);
}

Subscriber::~Subscriber() {
}

const string& Subscriber::getChannelName() {
	return _impl->getChannelName();
}

}
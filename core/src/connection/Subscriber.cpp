#include "connection/Subscriber.h"
#include "common/Factory.h"

namespace umundo {

Subscriber::Subscriber(std::string channelName, Receiver* receiver) {
	_impl = Factory::createSubscriber(channelName, receiver);
}

Subscriber::~Subscriber() {
}

string Subscriber::getChannelName() {
	return _impl->getChannelName();
}

}
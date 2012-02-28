#include "umundo/connection/Publisher.h"

#include "umundo/common/Factory.h"
#include "umundo/common/Message.h"

namespace umundo {

shared_ptr<Configuration> PublisherConfig::create() {
	return shared_ptr<Configuration>(new PublisherConfig());
}

Publisher::Publisher(const string& channelName) {
	_impl = boost::static_pointer_cast<PublisherImpl>(Factory::create("publisher"));
	_impl->setChannelName(channelName);
	shared_ptr<PublisherConfig> config = boost::static_pointer_cast<PublisherConfig>(Factory::config("publisher"));
	config->channelName = channelName;
	_impl->init(config);
}

void Publisher::send(const char* data, size_t length) {
	Message* msg = new Message(data, length);
	_impl->send(msg);
	delete(msg);
}

Publisher::~Publisher() {
}

}
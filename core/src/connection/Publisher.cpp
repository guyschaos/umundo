#include "connection/Publisher.h"
#include "common/Factory.h"

namespace umundo {

Publisher::Publisher(const string& channelName) : PublisherStub(channelName) {
	_impl = Factory::createPublisher(channelName);
}

Publisher::~Publisher() {
  delete _impl;
}

void Publisher::send(char* buffer, size_t length) {
	_impl->send(buffer, length);
}

}
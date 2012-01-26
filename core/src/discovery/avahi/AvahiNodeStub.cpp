#include "discovery/avahi/AvahiNodeStub.h"
#include "discovery/avahi/AvahiNodeDiscovery.h"

namespace umundo {

AvahiNodeStub::AvahiNodeStub() {
	_isRemote = false;
};

AvahiNodeStub::~AvahiNodeStub() {

}

uint16_t AvahiNodeStub::getPort() {
	resolve();
	return _port;
}

const string& AvahiNodeStub::getIP() {
	resolve();
	return (_interfaces.begin())->second;
}

const string& AvahiNodeStub::getDomain() {
	resolve();
	return _domain;
}

const string& AvahiNodeStub::getHost() {
	resolve();
	return _host;
}

void AvahiNodeStub::resolve() {
	if (!_isRemote)
		return;
}

std::ostream& operator<<(std::ostream &out, const AvahiNodeStub* n) {
	out << n->_uuid;
	return out;
}

}
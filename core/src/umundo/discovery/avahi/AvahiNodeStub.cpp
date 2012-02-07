#include "umundo/discovery/avahi/AvahiNodeStub.h"
#include "umundo/discovery/avahi/AvahiNodeDiscovery.h"

namespace umundo {

AvahiNodeStub::AvahiNodeStub() {
	_isRemote = false;
};

AvahiNodeStub::~AvahiNodeStub() {
}

const string& AvahiNodeStub::getIP() const {
	return (_interfaces.begin())->second;
}

std::ostream& operator<<(std::ostream &out, const AvahiNodeStub* n) {
	out << n->_uuid;
	return out;
}

}
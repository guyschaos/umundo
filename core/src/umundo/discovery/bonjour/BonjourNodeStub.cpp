#include "umundo/config.h"

#include <errno.h>

#if (defined UNIX || defined IOS || defined ANDROID)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#endif
#ifdef WIN32
#include <time.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

// we cannot include dns_sd.h before WinSock2.h
#include "umundo/discovery/bonjour/BonjourNodeStub.h"
#include "umundo/discovery/bonjour/BonjourNodeDiscovery.h"

namespace umundo {

BonjourNodeStub::BonjourNodeStub() {
	_isRemote = false;
	_isAdded = false;
};

BonjourNodeStub::~BonjourNodeStub() {
}

const string& BonjourNodeStub::getIP() const {
	// just return the first ip address that is not empty
	assert(_interfacesIPv4.size() > 0);
	map<int, string>::const_iterator ifIter = _interfacesIPv4.begin();
	while(ifIter != _interfacesIPv4.end()) {
		if (ifIter->second.length() > 0)
			return ifIter->second;
		ifIter++;
	}
	LOG_ERR("We did not found a valid address yet.");
	return _interfacesIPv4.begin()->second;
}

std::ostream& operator<<(std::ostream &out, const BonjourNodeStub* n) {
	out << n->_uuid << " (";
	std::set<std::string>::iterator domIter;
	for (domIter = n->_domains.begin(); domIter != n->_domains.end(); domIter++) {
		out << *domIter;
		if (domIter->npos < n->_domains.size())
			out << ", ";
	}
	out << ")";
	return out;
}

}

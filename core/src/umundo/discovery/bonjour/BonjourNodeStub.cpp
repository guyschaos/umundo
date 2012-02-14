#include "umundo/config.h"

#if (defined UNIX || defined IOS)
#include <arpa/inet.h>
#include <netinet/in.h>
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
	DEBUG_CTOR("BonjourNodeStub");
	_isRemote = false;
	_isAdded = false;
	_ttl = 0;
};

BonjourNodeStub::~BonjourNodeStub() {
	DEBUG_DTOR("BonjourNodeStub");
}

const string& BonjourNodeStub::getIP() const {
	// just return the first ip address
	assert(_interfaces.size() > 0);
	return (_interfaces.begin())->second;
}

void BonjourNodeStub::resolve() {
	_mutex.lock();
	// we already resolved
	if (time(NULL) < _ttl) {
		_mutex.unlock();
		return;
	}

	DNSServiceFlags rflags = 0;
	uint32_t interfaceIndex = kDNSServiceInterfaceIndexAny;
	intptr_t address = (intptr_t)(this);

	const char* name = (_uuid.length() ? _uuid.c_str() : NULL);

	char* regtype;
	assert(getTransport().length() > 0);
	asprintf(&regtype, "_mundo._%s", getTransport().c_str());

	const char* domain = (_domain.length() ? _domain.c_str() : "local.");

	/**
	 * Resolve service domain name, target hostname, port number and txt record
	 */
	int err = DNSServiceResolve(
	              &_dnsResolveClient,
	              rflags,
	              interfaceIndex,
	              name,
	              regtype,
	              domain,
	              resolveReply,
	              (void*)address
	          );

	if (err == kDNSServiceErr_NoError && _dnsResolveClient) {
		DNSServiceProcessResult(_dnsResolveClient);
	} else {
		LOG_WARN("DNSServiceResolve returned error");
	}

	rflags = kDNSServiceFlagsReturnIntermediates;
	uint32_t protocol = 0;
	err = DNSServiceGetAddrInfo(
	          &_dnsQueryClient,
	          rflags,       // kDNSServiceFlagsForceMulticast, kDNSServiceFlagsLongLivedQuery
	          interfaceIndex,
	          protocol,    // kDNSServiceProtocol_IPv4, kDNSServiceProtocol_IPv6
	          _hostTarget.c_str(),
	          addrInfoReply,
	          (void*)address
	      );

	if (err == kDNSServiceErr_NoError && _dnsQueryClient) {
		DNSServiceProcessResult(_dnsQueryClient);
	} else {
		LOG_WARN("DNSServiceQueryRecord returned error");
	}

	_ttl = time(NULL) + BONJOUR_RESOLVE_TTL;
	free(regtype);
	_mutex.unlock();

}

void DNSSD_API BonjourNodeStub::addrInfoReply(
    DNSServiceRef sdRef,
    DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *hostname,
    const struct sockaddr *address,
    uint32_t ttl,
    void *context
) {

#if DISC_BONJ_DEBUG
	std::cout << "addrInfoReply["
	          << " f:" << flags
	          << " if:" << interfaceIndex
	          << " err:" << errorCode
	          << " hostname:" << hostname
	          << " ttl:" << ttl
	          << "]" << std::endl;
#endif

	if (errorCode != kDNSServiceErr_NoError) {
//		LOG_WARN("BonjourNodeStub::addrInfoReply called with error");
		return;
	}

	bool override = false;
	char* addr = NULL;

	if (address && address->sa_family == AF_INET) {
		const unsigned char *b = (const unsigned char *) &((struct sockaddr_in *)address)->sin_addr;
		asprintf(&addr, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
		override = true; // we prefer ipv4 ..
	}	else if (address && address->sa_family == AF_INET6) {
		const struct sockaddr_in6 *s6 = (const struct sockaddr_in6 *)address;
		const unsigned char *b = (const unsigned char*)&s6->sin6_addr;

		asprintf(&addr, "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
		         b[0x0], b[0x1], b[0x2], b[0x3], b[0x4], b[0x5], b[0x6], b[0x7],
		         b[0x8], b[0x9], b[0xA], b[0xB], b[0xC], b[0xD], b[0xE], b[0xF]);
	}
#if DISC_BONJ_DEBUG
	std::cout << "addrInfoReply: " << hostname << " address: " << addr << std::endl;
#endif

	BonjourNodeStub* node = (BonjourNodeStub*)context;
	if (node->_interfaces.find(interfaceIndex) == node->_interfaces.end() || override)
		node->_interfaces[interfaceIndex] = addr;
	free(addr);
}

void DNSSD_API BonjourNodeStub::resolveReply(
    DNSServiceRef sdref,
    const DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *fullname,   // full service domain name: <servicename>.<protocol>.<domain>
    const char *hosttarget, // target hostname of the machine providing the service
    uint16_t opaqueport,    // port in network byte order
    uint16_t txtLen,        // length of the txt record
    const unsigned char *txtRecord, // primary txt record
    void *context
) {

#if DISC_BONJ_DEBUG
	std::cout << "resolveReply["
	          << " f:" << flags
	          << " if:" << interfaceIndex
	          << " err:" << errorCode
	          << " fullname:" << fullname
	          << " hosttarget:" << hosttarget
	          << " port:" << ntohs(opaqueport)
	          << " txtLen:" << txtLen
	          << " txtRecord:" << txtRecord
	          << "]" << std::endl;
#endif

	if(errorCode == kDNSServiceErr_NoError) {
		BonjourNodeStub* node = (BonjourNodeStub*)context;
		node->_bonjourDomain = fullname;
		node->_hostTarget = hosttarget;

		const char* domainStart = strchr(hosttarget, '.');
		node->_domain = ++domainStart;

		char* host = (char*)malloc(strlen(hosttarget) + 1);
		memcpy(host, hosttarget, strlen(hosttarget));
		char* hostEnd = strchr(host, '.');
		hostEnd[0] = 0;
		node->_host = host;
		free(host);

		node->_port = ntohs(opaqueport);
	} else {
		LOG_WARN("BonjourNodeStub::resolveReply called with error");
	}
}

std::ostream& operator<<(std::ostream &out, const BonjourNodeStub* n) {
	out << n->_uuid << " (";
	std::set<std::string>::iterator domIter;
	for (domIter = n->_actualDomains.begin(); domIter != n->_actualDomains.end(); domIter++) {
		out << *domIter;
		if (domIter->npos < n->_actualDomains.size())
			out << ", ";
	}
	out << ")";
	return out;
}

}
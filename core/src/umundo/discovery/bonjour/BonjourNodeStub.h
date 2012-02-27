#ifndef BonjourNodeStub_H_WRJ8277D
#define BonjourNodeStub_H_WRJ8277D

#include "dns_sd.h"

#include "umundo/common/NodeStub.h"
#include "umundo/thread/Thread.h"

#define BONJOUR_RESOLVE_TTL 120

namespace umundo {

/**
 * Concrete nodestub implementor for bonjour.
 */
class BonjourNodeStub : public NodeStub {
public:
	BonjourNodeStub();
	virtual ~BonjourNodeStub();

	/// Overwritten from EndPoint.
	const string& getIP() const;

private:
	void resolve();

	static void DNSSD_API resolveReply(
	    DNSServiceRef sdref,
	    const DNSServiceFlags flags,
	    uint32_t ifIndex,
	    DNSServiceErrorType errorCode,
	    const char *fullname,
	    const char *hosttarget,
	    uint16_t opaqueport,
	    uint16_t txtLen,
	    const unsigned char *txtRecord,
	    void *context
	);

	static void DNSSD_API addrInfoReply(
	    DNSServiceRef sdRef,
	    DNSServiceFlags flags,
	    uint32_t interfaceIndex,
	    DNSServiceErrorType errorCode,
	    const char *hostname,
	    const struct sockaddr *address,
	    uint32_t ttl,
	    void *context
	);

	DNSServiceRef _dnsRegisterClient;
	DNSServiceRef _dnsResolveClient;
	DNSServiceRef _dnsQueryClient;

	bool _isAdded;
	map<int, string> _interfaces;
	
	std::set<string> _domains;
	std::set<int> _interfaceIndices;
	string _regType;
	string _bonjourDomain;
	string _hostTarget;
	Mutex _mutex;
	time_t _ttl;

	friend std::ostream& operator<<(std::ostream&, const BonjourNodeStub*);
	friend class BonjourNodeDiscovery;
};

}

#endif /* end of include guard: BonjourNodeStub_H_WRJ8277D */

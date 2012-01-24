#ifndef BonjourNodeStub_H_WRJ8277D
#define BonjourNodeStub_H_WRJ8277D

#include "common/NodeStub.h"
#include "thread/Thread.h"
#include "dns_sd.h"

#include "common/stdInc.h"

#ifdef UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define BONJOUR_RESOLVE_TTL 120

namespace umundo {
	
	class BonjourNodeStub : public NodeStub {
	public:
		BonjourNodeStub();
		virtual ~BonjourNodeStub();

		uint16_t getPort();
		const string& getDomain();
		const string& getHost();
    const string& getIP();

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

#if 0
	static void queryReply(
	    DNSServiceRef sdRef,
	    DNSServiceFlags flags,
	    uint32_t ifIndex,
	    DNSServiceErrorType errorCode,
	    const char *fullname,
	    uint16_t rrtype,
	    uint16_t rrclass,
	    uint16_t rdlen,
	    const void *rdata,
	    uint32_t ttl,
	    void *context
	  );
#endif
    
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

		std::set<std::string> _actualDomains;
		std::string _regType;
    std::string _bonjourDomain;
    std::string _hostTarget;
    Mutex _mutex;
		time_t _ttl;

		friend std::ostream& operator<<(std::ostream&, const BonjourNodeStub*);		
		friend class BonjourNodeDiscovery;
	};
	
}

#endif /* end of include guard: BonjourNodeStub_H_WRJ8277D */

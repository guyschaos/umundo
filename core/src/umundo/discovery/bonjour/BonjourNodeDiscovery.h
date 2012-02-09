#ifndef DISCOVERER_H_94LKA4M1
#define DISCOVERER_H_94LKA4M1

#include "dns_sd.h"

/**
 * Keep in mind that the bonjour concept of service differs from umundo's. Bonjour services are
 * things like printers or web-servers. In this sense, we provide a umundo node as a bonjour service.
 */

#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"
#include "umundo/discovery/Discovery.h"

#define DISC_BONJ_DEBUG 0
#define BONJOUR_REPOLL_USEC 10000
#define BONJOUR_REPOLL_SEC 0

namespace umundo {

class NodeQuery;
class BonjourNodeStub;

/**
 * Concrete discovery implementor for Bonjour (bridge pattern).
 *
 * This class is a concrete implementor (in the bridge pattern sense) for the Discovery subsystem. It uses
 * Bonjour from Apple to realize node discovery within a multicast domain.
 * \see
 *	http://developer.apple.com/library/mac/#documentation/Networking/Reference/DNSServiceDiscovery_CRef/dns_sd_h/<br />
 *  http://developer.apple.com/library/mac/#documentation/networking/Conceptual/dns_discovery_api/Introduction.html<br />
 *  http://developer.apple.com/opensource/
 */
class BonjourNodeDiscovery : public DiscoveryImpl, public Thread {
public:
	virtual ~BonjourNodeDiscovery();
	static shared_ptr<BonjourNodeDiscovery> getInstance();  ///< Return the singleton instance.
	
	shared_ptr<Implementation> create();
	void destroy();
	void init(shared_ptr<Configuration>);

	void add(shared_ptr<NodeImpl> node);
	void remove(shared_ptr<NodeImpl> node);

	void browse(shared_ptr<NodeQuery> discovery);
	void unbrowse(shared_ptr<NodeQuery> discovery);

	void run();

private:
	BonjourNodeDiscovery();
	
	/** @name Bonjour callbacks */
  //@{
	static void DNSSD_API browseReply(
	    DNSServiceRef sdref,
	    const DNSServiceFlags flags,
	    uint32_t ifIndex,
	    DNSServiceErrorType errorCode,
	    const char *replyName,
	    const char *replyType,
	    const char *replyDomain,
	    void *context
	);

	static void DNSSD_API registerReply(
	    DNSServiceRef sdRef,
	    DNSServiceFlags flags,
	    DNSServiceErrorType errorCode,
	    const char* name,
	    const char* regtype,
	    const char* domain,
	    void* context
	);
  //@}

	
	map<int, DNSServiceRef> _sockFD;          ///< Socket file descriptors to bonjour handle.
	map<intptr_t, shared_ptr<NodeQuery> > _browsers;     ///< Memory addresses to node queries for static callbacks.
	map<intptr_t, shared_ptr<NodeImpl> > _nodes;  ///< Memory addresses of local nodes for static callbacks.
	map<intptr_t, DNSServiceRef> _dnsClients; ///< Bonjour handles for local node registration.

	/// All the nodes a query was notified about.
	map<shared_ptr<NodeQuery>, map<string, shared_ptr<BonjourNodeStub> > > _queryNodes;
	Mutex _mutex;

	static shared_ptr<BonjourNodeDiscovery> _instance;  ///< The singleton instance.

	friend class BonjourNodeStub;
	friend class Factory;
};

}

#endif /* end of include guard: DISCOVERER_H_94LKA4M1 */
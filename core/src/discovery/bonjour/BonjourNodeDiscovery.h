#ifndef DISCOVERER_H_94LKA4M1
#define DISCOVERER_H_94LKA4M1

#include "common/stdInc.h"
/**
 * Keep in mind that the bonjour concept of service differs from umundo's. Bonjour services are
 * things like printers or web-servers. In this sense, we provide a umundo node as a bonjour service.
 */
#include "dns_sd.h"

#include "common/Node.h"
#include "thread/Thread.h"
#include "discovery/Discovery.h"
#include "discovery/NodeQuery.h"
#include "discovery/bonjour/BonjourNodeStub.h"

#define DISC_BONJ_DEBUG 0
#define BONJOUR_REPOLL_USEC 10000
#define BONJOUR_REPOLL_SEC 0

namespace umundo {

class BonjourNodeDiscovery : public DiscoveryImpl, public Thread {
public:
	BonjourNodeDiscovery();
	virtual ~BonjourNodeDiscovery();
	static BonjourNodeDiscovery* getInstance();

	DiscoveryImpl* create();
		
	void remove(shared_ptr<Node> node);
	void add(shared_ptr<Node> node);
	
	void browse(NodeQuery* discovery);
	void unbrowse(NodeQuery* discovery);
  void run();

private:
  // static callbacks for bonjour
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
  
	map<int, DNSServiceRef> _sockFD;                  // socket file descriptors to bonjour handle
	map<intptr_t, NodeQuery* > _browsers;             // memory addresses of queries for static callbacks
	map<intptr_t, shared_ptr<Node> > _nodes;	        // memory addresses of local nodes for static callbacks
	map<intptr_t, DNSServiceRef> _dnsClients;	        // bonjour handles for local node registration

	map<NodeQuery*, map<string, shared_ptr<BonjourNodeStub> > > _queryNodes;
  Mutex _mutex;
	
	static BonjourNodeDiscovery* _instance;
	
	friend class BonjourNodeStub;
	friend class Factory;
};

}

#endif /* end of include guard: DISCOVERER_H_94LKA4M1 */

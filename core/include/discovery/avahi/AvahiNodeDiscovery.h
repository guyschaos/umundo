#ifndef AVAHINODEDISCOVERY_H_GCM9GM15
#define AVAHINODEDISCOVERY_H_GCM9GM15

#include "common/stdInc.h"
#include "common/Node.h"
#include "thread/Thread.h"
#include "discovery/Discovery.h"
#include "discovery/NodeQuery.h"
#include "discovery/avahi/AvahiNodeStub.h"

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/error.h>
#include <avahi-common/defs.h>
#include <avahi-common/malloc.h>

#define AVAHI_WARN(x, err) AvahiNodeDiscovery::printWarn(__FILE__, __LINE__, x, err);

#define DISC_AVAHI_DEBUG 0

namespace umundo {

class AvahiNodeDiscovery : public DiscoveryImpl, public Thread {
public:
	virtual ~AvahiNodeDiscovery();
	static AvahiNodeDiscovery* getInstance();

	DiscoveryImpl* create();

	void remove(shared_ptr<Node> node);
	void add(shared_ptr<Node> node);

	void browse(NodeQuery* discovery);
	void unbrowse(NodeQuery* discovery);
	void run();

private:
	AvahiNodeDiscovery();

	AvahiSimplePoll *_simplePoll;

	static void entryGroupCallback(AvahiEntryGroup*, AvahiEntryGroupState, void*);
	static void clientCallback(AvahiClient*, AvahiClientState, void*);
	static void browseClientCallback(AvahiClient*, AvahiClientState, void*);

	static void browseCallback(
	    AvahiServiceBrowser *b,
	    AvahiIfIndex interface,
	    AvahiProtocol protocol,
	    AvahiBrowserEvent event,
	    const char *name,
	    const char *type,
	    const char *domain,
	    AvahiLookupResultFlags flags,
	    void* userdata
	);

	static void resolveCallback(
	    AvahiServiceResolver *r,
	    AvahiIfIndex interface,
	    AvahiProtocol protocol,
	    AvahiResolverEvent event,
	    const char *name,
	    const char *type,
	    const char *domain,
	    const char *host_name,
	    const AvahiAddress *address,
	    uint16_t port,
	    AvahiStringList *txt,
	    AvahiLookupResultFlags flags,
	    void* userdata
	);

	map<intptr_t, NodeQuery* > _browsers;       // memory addresses of queries for static callbacks
	map<intptr_t, shared_ptr<Node> > _nodes;	             // memory addresses of local nodes for static callbacks
	map<intptr_t, AvahiClient* > _avahiClients;            // memory addresses of local nodes to avahi clients
	map<intptr_t, AvahiEntryGroup* > _avahiGroups;         // memory addresses of local nodes to avahi groups
	map<intptr_t, AvahiServiceBrowser* > _avahiBrowser;        // memory addresses of local nodes to avahi service browsers

	map<NodeQuery*, map<string, shared_ptr<AvahiNodeStub> > > _queryNodes;

	static AvahiNodeDiscovery* _instance;

	static bool printWarn(const char* file, int line, const char* fct, int err);

	friend class AvahiNodeStub;
	friend class Factory;
};

}

#endif /* end of include guard: AVAHINODEDISCOVERY_H_GCM9GM15 */

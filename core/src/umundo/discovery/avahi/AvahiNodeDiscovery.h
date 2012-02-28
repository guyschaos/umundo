#ifndef AVAHINODEDISCOVERY_H_GCM9GM15
#define AVAHINODEDISCOVERY_H_GCM9GM15

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>

#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"
#include "umundo/discovery/Discovery.h"

namespace umundo {

class AvahiNodeStub;

/**
 * Concrete discovery implementor for avahi (bridge pattern).
 */
class AvahiNodeDiscovery : public DiscoveryImpl, public Thread {
public:
	virtual ~AvahiNodeDiscovery();
	static shared_ptr<AvahiNodeDiscovery> getInstance();

	shared_ptr<Implementation> create();
	void destroy();
	void init(shared_ptr<Configuration>);

	void add(shared_ptr<NodeImpl> node);
	void remove(shared_ptr<NodeImpl> node);

	void browse(shared_ptr<NodeQuery> discovery);
	void unbrowse(shared_ptr<NodeQuery> discovery);

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

	map<intptr_t, shared_ptr<NodeQuery> > _browsers;              ///< memory addresses of queries for static callbacks
	map<intptr_t, shared_ptr<NodeImpl> > _nodes;	             ///< memory addresses of local nodes for static callbacks
	map<intptr_t, AvahiClient* > _avahiClients;            ///< memory addresses of local nodes to avahi clients
	map<intptr_t, AvahiEntryGroup* > _avahiGroups;         ///< memory addresses of local nodes to avahi groups
	map<intptr_t, AvahiServiceBrowser* > _avahiBrowser;        ///< memory addresses of local nodes to avahi service browsers

	map<shared_ptr<NodeQuery>, map<string, shared_ptr<AvahiNodeStub> > > _queryNodes;

	static shared_ptr<AvahiNodeDiscovery> _instance;

	friend class AvahiNodeStub;
	friend class Factory;
};

}

#endif /* end of include guard: AVAHINODEDISCOVERY_H_GCM9GM15 */

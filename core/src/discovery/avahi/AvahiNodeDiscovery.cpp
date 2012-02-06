#include "discovery/avahi/AvahiNodeDiscovery.h"

#include "common/Node.h"
#include "discovery/NodeQuery.h"
#include "discovery/avahi/AvahiNodeStub.h"
#include <avahi-common/error.h>
#include <avahi-common/malloc.h>

namespace umundo {

shared_ptr<Implementation> AvahiNodeDiscovery::create() {
	return getInstance();
}

shared_ptr<AvahiNodeDiscovery> AvahiNodeDiscovery::getInstance() {
	if (_instance.get() == NULL) {
		_instance = shared_ptr<AvahiNodeDiscovery>(new AvahiNodeDiscovery());
		(_instance->_simplePoll = avahi_simple_poll_new()) || LOG_WARN("avahi_simple_poll_new", 0);
	}
	return _instance;
}
shared_ptr<AvahiNodeDiscovery> AvahiNodeDiscovery::_instance;

AvahiNodeDiscovery::AvahiNodeDiscovery() {
}

void AvahiNodeDiscovery::destroy() {
}

void AvahiNodeDiscovery::init(shared_ptr<Configuration>) {
	LOG_WARN("AvahiNodeDiscovery::init avahi_simple_poll_new")
}

AvahiNodeDiscovery::~AvahiNodeDiscovery() {
}

void AvahiNodeDiscovery::remove(shared_ptr<NodeImpl> node) {
	/// @todo Implement node removal for avahi discovery.
	assert(false);
}

void AvahiNodeDiscovery::add(shared_ptr<NodeImpl> node) {
	int err;
	intptr_t address = (intptr_t)(node.get());
	getInstance()->_nodes[address] = node;

	AvahiClient* client = avahi_client_new(avahi_simple_poll_get(_simplePoll), (AvahiClientFlags)0, &clientCallback, (void*)address, &err);
	if (!client) LOG_WARN("avahi_client_new", err);
	getInstance()->_avahiClients[address] = client;
	getInstance()->start();
}

void AvahiNodeDiscovery::unbrowse(shared_ptr<NodeQuery> query) {
	/// @todo Implement query removal for avahi discovery.
	assert(false);
}

void AvahiNodeDiscovery::browse(shared_ptr<NodeQuery> query) {
	AvahiServiceBrowser *sb = NULL;
	AvahiClient *client = NULL;
	intptr_t address = (intptr_t)(query.get());
	int error;

	client = avahi_client_new(avahi_simple_poll_get(_simplePoll), (AvahiClientFlags)0, browseClientCallback, (void*)address, &error);
	if (client == NULL) {
		LOG_WARN("avahi_client_new failed", error);
		return;
	}
	getInstance()->_avahiClients[address] = client;
	getInstance()->_browsers[address] = query;

	if (!(sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_mundo._tcp", NULL, (AvahiLookupFlags)0, browseCallback, (void*)address))) {
		LOG_WARN("avahi_service_browser_new failed", error);
	}
	getInstance()->start();	
}

void AvahiNodeDiscovery::browseClientCallback(AvahiClient *c, AvahiClientState state, void * userdata) {
	assert(c);

	switch(state) {
	case AVAHI_CLIENT_CONNECTING:
		LOG_WARN("Client still connecting", avahi_client_errno(c));
		break;
	case AVAHI_CLIENT_FAILURE:
		LOG_WARN("Server connection failure", avahi_client_errno(c));
		break;
	case AVAHI_CLIENT_S_RUNNING:
		//LOG_WARN("Server state: RUNNING", avahi_client_errno(c));
		break;
	case AVAHI_CLIENT_S_REGISTERING:
		LOG_WARN("Server state: REGISTERING", avahi_client_errno(c));
		break;
	case AVAHI_CLIENT_S_COLLISION:
		LOG_WARN("Server state: COLLISION", avahi_client_errno(c));
		break;
	}
}

void AvahiNodeDiscovery::browseCallback(
    AvahiServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AvahiLookupResultFlags flags,
    void* userdata
) {
#if DISC_AVAHI_DEBUG
	std::cout << "browseCallback["
	          << "\n\tif:" << interface
	          << " proto:" << protocol
	          << " event:" << event
	          << " name:" << (name == NULL ? "NULL" : name)
	          << " type:" << type
	          << " domain:" << (domain == NULL ? "NULL" : domain)
	          << " f:" << flags
	          << "]" << std::endl;
#endif

	shared_ptr<AvahiNodeDiscovery> myself = getInstance();
	shared_ptr<NodeQuery> query = myself->_browsers[(intptr_t)userdata];
	AvahiClient* client = myself->_avahiClients[(intptr_t)userdata];

	switch (event) {
	case AVAHI_BROWSER_NEW: {
		// someone is announcing a new node
		assert(name != NULL);
		shared_ptr<AvahiNodeStub> node = myself->_queryNodes[query][name];
		bool knownNode = (node.get() != NULL);

		if (!knownNode) {
			// we found ourselves a new node
			node = shared_ptr<AvahiNodeStub>(new AvahiNodeStub());
			node->_transport = "tcp";
			node->_uuid = name;
			node->_domain = domain;
			node->_interfaces[interface] = "";
			myself->_queryNodes[query][name] = node;
		}
		if (!(avahi_service_resolver_new(client, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, (AvahiLookupFlags)0, resolveCallback, userdata)))
			LOG_WARN("avahi_service_resolver_new failed", avahi_client_errno(client));
		break;
	}
	case AVAHI_BROWSER_REMOVE: {
		assert(name != NULL);
		shared_ptr<AvahiNodeStub> node = myself->_queryNodes[query][name];
		query->removed(node);
		// TODO: remove from query nodes
		break;
	}
	case AVAHI_BROWSER_CACHE_EXHAUSTED:
		break;
	case AVAHI_BROWSER_ALL_FOR_NOW: {
		// TODO: This is a better place to notify the query listener
		break;
	}
	case AVAHI_BROWSER_FAILURE:
		LOG_WARN("avahi browser failure", avahi_client_errno(avahi_service_browser_get_client(b)));
		break;
	}
}

void AvahiNodeDiscovery::resolveCallback(
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
) {
#if DISC_AVAHI_DEBUG
	std::cout << "resolveCallback["
	          << "\n\tif:" << interface
	          << " proto:" << protocol
	          << " event:" << event
	          << " name:" << (name == NULL ? "NULL" : name)
	          << " type:" << type
	          << " domain:" << (domain == NULL ? "NULL" : domain)
	          << " hostname:" << (host_name == NULL ? "NULL" : host_name)
	          << " addr:" << address
	          << " port:" << port
	          << " txt:" << (txt == NULL ? "NULL" : avahi_string_list_to_string(txt))
	          << " f:" << flags
	          << "]" << std::endl;
#endif

	shared_ptr<AvahiNodeDiscovery> myself = getInstance();
	shared_ptr<NodeQuery> query = myself->_browsers[(intptr_t)userdata];
	AvahiClient* client = myself->_avahiClients[(intptr_t)userdata];
	shared_ptr<AvahiNodeStub> node = myself->_queryNodes[query][name];

	assert(query);
	assert(client);
	assert(node);

	if (protocol == AVAHI_PROTO_INET6) {
		LOG_WARN("Ignoring %s IPv6", host_name);
		return;
	}

	switch (event) {
	case AVAHI_RESOLVER_FAILURE:
		LOG_WARN("resolving failed", avahi_client_errno(avahi_service_resolver_get_client(r)));
		break;

	case AVAHI_RESOLVER_FOUND: {
		char addr[AVAHI_ADDRESS_STR_MAX], *t;
		t = avahi_string_list_to_string(txt);
		avahi_address_snprint(addr, sizeof(addr), address);
		node->_interfaces[interface] = addr;
		node->_isRemote = !(flags & AVAHI_LOOKUP_RESULT_LOCAL);
		node->_isOurOwn = !(flags & AVAHI_LOOKUP_RESULT_OUR_OWN);
		node->_isWan = !(flags & AVAHI_LOOKUP_RESULT_WIDE_AREA);
		node->_txtRecord = t;
		node->_host = host_name;
		node->_port = port;

		avahi_free(t);
		// this is the wrong place to notify the query listener ...
		query->added(node);

		break;
	}
	default:
		LOG_WARN("Unknown event in resolveCallback", 0);
	}

	avahi_service_resolver_free(r);
}

void AvahiNodeDiscovery::entryGroupCallback(AvahiEntryGroup *g, AvahiEntryGroupState state, void* userdata) {
	shared_ptr<AvahiNodeDiscovery> myself = getInstance();
	shared_ptr<NodeImpl> node = myself->_nodes[(intptr_t)userdata];
	AvahiEntryGroup* group = myself->_avahiGroups[(intptr_t)userdata];

	assert(g == group || group == NULL);
	myself->_avahiGroups[(intptr_t)userdata] = g;

	/* Called whenever the entry group state changes */
	switch (state) {
	case AVAHI_ENTRY_GROUP_ESTABLISHED :
		/* The entry group has been established successfully */
		//std::cout << "Service successfully established: " << node->getUUID() << std::endl;
		break;

	case AVAHI_ENTRY_GROUP_COLLISION :
		LOG_WARN("AVAHI_ENTRY_GROUP_COLLISION", 0);
		assert(false);
		break;

	case AVAHI_ENTRY_GROUP_FAILURE :
		LOG_WARN("AVAHI_ENTRY_GROUP_FAILURE", avahi_client_errno(avahi_entry_group_get_client(g)));

		/* Some kind of failure happened while we were registering our services */
		avahi_simple_poll_quit(myself->_simplePoll);
		break;

	case AVAHI_ENTRY_GROUP_UNCOMMITED:
		//LOG_WARN("AVAHI_ENTRY_GROUP_UNCOMMITED", 0);
		break;
	case AVAHI_ENTRY_GROUP_REGISTERING:
		//LOG_WARN("AVAHI_ENTRY_GROUP_REGISTERING", 0);
		break;
	default:
		LOG_WARN("entryGroupCallback default switch", 0);

	}
}

void AvahiNodeDiscovery::clientCallback(AvahiClient* c, AvahiClientState state, void* userdata) {
	assert(c);
	int err;
	shared_ptr<AvahiNodeDiscovery> myself = getInstance();
	shared_ptr<NodeImpl> node = myself->_nodes[(intptr_t)userdata];
	AvahiEntryGroup* group = myself->_avahiGroups[(intptr_t)userdata];
	assert(node.get() != NULL);

	switch (state) {
	case AVAHI_CLIENT_S_RUNNING:
		/* The server has startup successfully and registered its host
		 * name on the network, so it's time to create our services */
		if (!group) {
			if (!(group = avahi_entry_group_new(c, entryGroupCallback, userdata))) {
				LOG_WARN("avahi_entry_group_new failed ", avahi_client_errno(c));
				avahi_simple_poll_quit(myself->_simplePoll);
			} else {
				myself->_avahiGroups[(intptr_t)userdata] = group;
			}
		}
		if (avahi_entry_group_is_empty(group)) {
			if ((err = avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, node->getUUID().c_str(), "_mundo._tcp", NULL, NULL, node->getPort(), NULL)) < 0) {
				if (err == AVAHI_ERR_COLLISION) {
					assert(0); // we register uuids, there shouldn't be collisions
				}
				LOG_WARN("avahi_entry_group_add_service failed ", err);
				avahi_simple_poll_quit(myself->_simplePoll);
			}

			/* Tell the server to register the service */
			if ((err = avahi_entry_group_commit(group)) < 0) {
				LOG_WARN("avahi_entry_group_commit failed ", err);
				avahi_simple_poll_quit(myself->_simplePoll);
			}
		}
		break;

	case AVAHI_CLIENT_FAILURE:
		LOG_WARN("Client failure ", avahi_client_errno(c));
		avahi_simple_poll_quit(myself->_simplePoll);
		break;

	case AVAHI_CLIENT_S_COLLISION:
		LOG_WARN("AVAHI_CLIENT_S_COLLISION ", avahi_client_errno(c));
		break;
	case AVAHI_CLIENT_S_REGISTERING:
		/* The server records are now being established. This
		 * might be caused by a host name change. We need to wait
		 * for our own records to register until the host name is
		 * properly esatblished. */
		LOG_WARN("AVAHI_CLIENT_S_REGISTERING ", avahi_client_errno(c));
		if (group)
			avahi_entry_group_reset(group);
		break;

	case AVAHI_CLIENT_CONNECTING:
		LOG_WARN("AVAHI_CLIENT_CONNECTING ", avahi_client_errno(c));
		break;
	}
}

void AvahiNodeDiscovery::run() {
	while (isStarted()) {
		Thread::sleepMs(500);
		avahi_simple_poll_loop(_simplePoll);
	}
}

}

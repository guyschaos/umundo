#include "umundo/discovery/avahi/AvahiNodeDiscovery.h"

#include <stdio.h> // asprintf

#include "umundo/common/Node.h"
#include "umundo/discovery/NodeQuery.h"
#include "umundo/discovery/avahi/AvahiNodeStub.h"
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
	_mutex.lock();
	LOG_DEBUG("Removing node %s", node->getUUID().c_str());
	intptr_t address = (intptr_t)(node.get());
	if(_avahiClients.find(address) == _avahiClients.end()) {
		LOG_WARN("Ignoring removal of unregistered node from discovery");
		_mutex.unlock();
		return;
	}
	avahi_client_free(_avahiClients[address]);
	_avahiClients.erase(address);
	_mutex.unlock();
}

void AvahiNodeDiscovery::add(shared_ptr<NodeImpl> node) {
	_mutex.lock();
	int err;
	intptr_t address = (intptr_t)(node.get());
	_nodes[address] = node;

	AvahiClient* client = avahi_client_new(avahi_simple_poll_get(_simplePoll), (AvahiClientFlags)0, &clientCallback, (void*)address, &err);
	if (!client) LOG_WARN("avahi_client_new - is the Avahi daemon running?", err);
	_avahiClients[address] = client;
	start();
	_mutex.unlock();
}

void AvahiNodeDiscovery::unbrowse(shared_ptr<NodeQuery> query) {
	_mutex.lock();
	intptr_t address = (intptr_t)(query.get());

	if(_avahiClients.find(address) == _avahiClients.end()) {
		LOG_WARN("Unbrowsing query that was never added");
		_mutex.unlock();
		return;
	}
	avahi_client_free(_avahiClients[address]);
	_avahiClients.erase(address);
	_queryNodes.erase(query);
	_mutex.unlock();
}

void AvahiNodeDiscovery::browse(shared_ptr<NodeQuery> query) {
	_mutex.lock();
	AvahiServiceBrowser *sb = NULL;
	AvahiClient *client = NULL;
	intptr_t address = (intptr_t)(query.get());
	int error;

	client = avahi_client_new(avahi_simple_poll_get(_simplePoll), (AvahiClientFlags)0, browseClientCallback, (void*)address, &error);
	if (client == NULL) {
		LOG_WARN("avahi_client_new failed - is the Avahi daemon running?", error);
		_mutex.unlock();
		return;
	}
	_avahiClients[address] = client;
	_browsers[address] = query;

	char* domain;
	if (query->getDomain().length() > 0) {
		asprintf(&domain, "%s.local.", query->getDomain().c_str());
	} else {
		asprintf(&domain, "local.");
	}

	if (!(sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, "_mundo._tcp", domain, (AvahiLookupFlags)0, browseCallback, (void*)address))) {
		LOG_WARN("avahi_service_browser_new failed", error);
	}
	free(domain);
	start();
	_mutex.unlock();
}

void AvahiNodeDiscovery::browseClientCallback(AvahiClient *c, AvahiClientState state, void * userdata) {
	getInstance()->_mutex.lock();
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
	getInstance()->_mutex.unlock();
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
	getInstance()->_mutex.lock();
	LOG_DEBUG("browseCallback: %s %s/%s as %s at if %d with protocol %d",
	          (event == AVAHI_BROWSER_NEW ? "Added" : "Called for"),
	          (name == NULL ? "NULL" : name),
	          (domain == NULL ? "NULL" : domain),
	          type,
	          interface,
	          protocol);

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
	getInstance()->_mutex.unlock();
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
	getInstance()->_mutex.lock();
	LOG_DEBUG("resolveCallback: %s %s/%s:%d as %s at if %d with protocol %d",
	          (host_name == NULL ? "NULL" : host_name),
	          (name == NULL ? "NULL" : name),
	          (domain == NULL ? "NULL" : domain),
	          port,
	          type,
	          interface,
	          protocol);

	shared_ptr<AvahiNodeDiscovery> myself = getInstance();
	shared_ptr<NodeQuery> query = myself->_browsers[(intptr_t)userdata];
	AvahiClient* client = myself->_avahiClients[(intptr_t)userdata];
	shared_ptr<AvahiNodeStub> node = myself->_queryNodes[query][name];

	(query.get() != NULL) || LOG_ERR("no browser known for userdata");
	(client != NULL) || LOG_ERR("no client known for userdata");
	(node != NULL) || LOG_ERR("no node named %s known for query", name);

	if (protocol == AVAHI_PROTO_INET6) {
		LOG_INFO("Ignoring %s IPv6", host_name);
		getInstance()->_mutex.unlock();
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
		query->found(node);

		break;
	}
	default:
		LOG_WARN("Unknown event in resolveCallback", 0);
	}

	avahi_service_resolver_free(r);
	getInstance()->_mutex.unlock();
}

void AvahiNodeDiscovery::entryGroupCallback(AvahiEntryGroup *g, AvahiEntryGroupState state, void* userdata) {
	getInstance()->_mutex.lock();
	shared_ptr<AvahiNodeDiscovery> myself = getInstance();
	shared_ptr<NodeImpl> node = myself->_nodes[(intptr_t)userdata];
	AvahiEntryGroup* group = myself->_avahiGroups[(intptr_t)userdata];

	(g == group || group == NULL) || LOG_ERR("Unknown group received");
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
	getInstance()->_mutex.unlock();
}

void AvahiNodeDiscovery::clientCallback(AvahiClient* c, AvahiClientState state, void* userdata) {
	getInstance()->_mutex.lock();
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
			char* domain;
			if (node->getDomain().length() > 0) {
				asprintf(&domain, "%s.local.", node->getDomain().c_str());
			} else {
				asprintf(&domain, "local.");
			}

			if ((err = avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, node->getUUID().c_str(), "_mundo._tcp", domain, NULL, node->getPort(), NULL)) < 0) {
				if (err == AVAHI_ERR_COLLISION) {
					assert(0); // we register uuids, there shouldn't be collisions
				}
				LOG_WARN("avahi_entry_group_add_service failed ", err);
				avahi_simple_poll_quit(myself->_simplePoll);
			}
			free(domain);
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
	getInstance()->_mutex.unlock();
}

void AvahiNodeDiscovery::run() {
	while (isStarted()) {
		_mutex.lock();
		int timeoutMs = 50;
		avahi_simple_poll_iterate(_simplePoll, timeoutMs) && LOG_WARN("avahi_simple_poll_iterate: %d", errno);
		_mutex.unlock();
		Thread::yield();
	}
}

}

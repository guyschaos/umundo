#include "umundo/config.h"

#ifdef DISC_BONJOUR_EMBED
extern "C" {
	int embedded_mDNS_Init();
}
#endif

#include "umundo/discovery/bonjour/BonjourNodeDiscovery.h"

#include <errno.h>

#if defined ANDROID
#include <arpa/inet.h>
#include <netinet/in.h>
#endif


#include "umundo/common/Node.h"
#include "umundo/discovery/NodeQuery.h"
#include "umundo/discovery/bonjour/BonjourNodeStub.h"

namespace umundo {

BonjourNodeDiscovery::BonjourNodeDiscovery() {
	DEBUG_CTOR("BonjourNodeDiscovery");
#ifdef DISC_BONJOUR_EMBED
	embedded_mDNS_Init();
#endif
}

shared_ptr<Implementation> BonjourNodeDiscovery::create() {
	return getInstance();
}

void BonjourNodeDiscovery::destroy() {
	// do nothing?
}

void BonjourNodeDiscovery::init(shared_ptr<Configuration>) {
	// do nothing?
}

shared_ptr<BonjourNodeDiscovery> BonjourNodeDiscovery::getInstance() {
	if (_instance.get() == NULL) {
		_instance = shared_ptr<BonjourNodeDiscovery>(new BonjourNodeDiscovery());
		_instance->start();
	}
	return _instance;
}
shared_ptr<BonjourNodeDiscovery> BonjourNodeDiscovery::_instance;

BonjourNodeDiscovery::~BonjourNodeDiscovery() {
	DEBUG_DTOR("BonjourNodeDiscovery");
}

void BonjourNodeDiscovery::remove(shared_ptr<NodeImpl> node) {
	intptr_t address = (intptr_t)(node.get());

	if (_nodes.find(address) == _nodes.end())
		return;

	_mutex.lock();
	DNSServiceRef dnsRegisterClient = _dnsClients[address];
	assert(_sockFD.find(DNSServiceRefSockFD(dnsRegisterClient)) != _sockFD.end());
	assert(_sockFD[DNSServiceRefSockFD(dnsRegisterClient)] == dnsRegisterClient);

	_dnsClients.erase(address);
	_sockFD.erase(DNSServiceRefSockFD(dnsRegisterClient));
	_nodes.erase(address);
	DNSServiceRefDeallocate(dnsRegisterClient);
	_mutex.unlock();
}

void BonjourNodeDiscovery::add(shared_ptr<NodeImpl> node) {
	DNSServiceErrorType err;
	DNSServiceRef dnsRegisterClient;

	assert(node->getUUID().length() > 0);

	DNSServiceFlags flags = 0; // kDNSServiceFlagsNoAutoRename;
	uint32_t interfaceIndex = kDNSServiceInterfaceIndexAny;
	const char* name = (node->getUUID().length() ? node->getUUID().c_str() : NULL);

	char* regtype;
	const char* transport = (node->getTransport().length() ? node->getTransport().c_str() : "tcp");
	asprintf(&regtype, "_mundo._%s", transport);

	char* domain;
	if (node->getDomain().length() > 0) {
		asprintf(&domain, "%s.local.", node->getDomain().c_str());
	} else {
		asprintf(&domain, "local.");
	}

	const char* host = (node->getHost().length() ? node->getHost().c_str() : NULL);
	uint16_t port = (node->getPort() > 0 ? htons(node->getPort()) : 0);
	const char* txtRecord = "";
	uint16_t txtLength = 0;

	intptr_t address = (intptr_t)(node.get());

	//std::cout << "registering name: " << name << " domain: " << domain << " host:" << host << " regtype" << regtype << std::endl;

	err = DNSServiceRegister(
	          &dnsRegisterClient,              // uninitialized DNSServiceRef
	          flags,                           // renaming behavior on name conflict
	          interfaceIndex,                  // If non-zero, specifies the interface, defaults to all interfaces
	          name,                            // If non-NULL, specifies the service name, defaults to computer name
	          regtype,                         // service type followed by the protocol
	          domain,                          // If non-NULL, specifies the domain, defaults to default domain
	          host,                            // If non-NULL, specifies the SRV target host name
	          port,                            // port number, defaults to name-reserving/non-discoverable
	          txtLength,                       // length of the txtRecord, in bytes
	          txtRecord,                       // TXT record rdata: <length byte> <data> <length byte> <data> ...
	          registerReply,                   // called when the registration completes
	          (void*)address                   // context pointer which is passed to the callback
	      );

	free(regtype);
	free(domain);

	_mutex.lock();
	if(dnsRegisterClient && err == 0) {
		int sockFD = DNSServiceRefSockFD(dnsRegisterClient);
		assert(getInstance()->_dnsClients[address] == NULL);
		getInstance()->_nodes[address] = node;
		getInstance()->_dnsClients[address] = dnsRegisterClient;
		getInstance()->_sockFD[sockFD] = dnsRegisterClient;
	} else {
		LOG_WARN("DNSServiceRegister returned error %d", err);
	}
	_mutex.unlock();
}

void BonjourNodeDiscovery::unbrowse(shared_ptr<NodeQuery> query) {
	intptr_t address = (intptr_t)(query.get());

	if (_dnsClients.find(address) == _dnsClients.end())
		return;

	_mutex.lock();
	DNSServiceRef dnsRegisterClient = _dnsClients[address];
	assert(_sockFD.find(DNSServiceRefSockFD(dnsRegisterClient)) != _sockFD.end());
	assert(_sockFD[DNSServiceRefSockFD(dnsRegisterClient)] == dnsRegisterClient);
	_dnsClients.erase(address);
	_sockFD.erase(DNSServiceRefSockFD(dnsRegisterClient));
	if (_queryNodes.find(query) != _queryNodes.end())
		_queryNodes.erase(query);
	_browsers.erase(address);

	DNSServiceRefDeallocate(dnsRegisterClient);
	_mutex.unlock();
}

void BonjourNodeDiscovery::browse(shared_ptr<NodeQuery> query) {
	DNSServiceErrorType err;
	DNSServiceRef client = NULL;
	DNSServiceFlags flags = 0;
	uint32_t opinterface = kDNSServiceInterfaceIndexAny;

	char* regtype;
	const char* transport = (query->getTransport().length() ? query->getTransport().c_str() : "tcp");
	asprintf(&regtype, "_mundo._%s", transport);

	char* domain;
	if (query->getDomain().length() > 0) {
		asprintf(&domain, "%s.local.", query->getDomain().c_str());
	} else {
		asprintf(&domain, "local.");
	}

	intptr_t address = (intptr_t)(query.get());

	query->notifyImmediately(false);
	getInstance()->_browsers[address] = query;

	err = DNSServiceBrowse(
	          &client,                       // uninitialized DNSServiceRef
	          flags,                         // Currently ignored, reserved for future use
	          opinterface,                   // non-zero, specifies the interface
	          regtype,                       // service type being browsed
	          domain,                        // non-NULL, specifies the domain
	          browseReply,                   // called when a service is found
	          (void*)address
	      );

	if(client && err == 0) {
		int sockFD = DNSServiceRefSockFD(client);
		assert(getInstance()->_dnsClients[address] == NULL);
		getInstance()->_dnsClients[address] = client;
		getInstance()->_sockFD[sockFD] = client;
	} else {
		LOG_WARN("DNSServiceBrowse returned error %d", err);
	}

	free(regtype);
	free(domain);
}

void BonjourNodeDiscovery::run() {
	fd_set readfds;
	struct timeval tv;

	int nfds = -1;

	while(isStarted()) {
		nfds = 0;
		FD_ZERO(&readfds);
		// tv structure gets modified we have to reset each time
		tv.tv_sec  = BONJOUR_REPOLL_SEC;
		tv.tv_usec = BONJOUR_REPOLL_USEC;

		std::map<int, DNSServiceRef>::const_iterator it;
		for (it = _sockFD.begin(); it != _sockFD.end(); it++) {
			if (it->first > nfds)
				nfds = it->first;
			FD_SET(it->first, &readfds);
		}
		nfds++;

		int result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);

		if (result > 0) {
			_mutex.lock();
			for (it = _sockFD.begin(); it != _sockFD.end(); it++) {
				if (FD_ISSET(it->first, &readfds)) {
					DNSServiceProcessResult(it->second);
				}
			}
			_mutex.unlock();
		} else if (result == 0) {
			// timeout
		} else {
			if (errno != 0)
				LOG_WARN("select failed %s", strerror(errno));
			Thread::sleepMs(500);
		}
	}
}

void DNSSD_API BonjourNodeDiscovery::browseReply(
    DNSServiceRef sdref,
    const DNSServiceFlags flags,
    uint32_t ifIndex,
    DNSServiceErrorType errorCode,
    const char *replyName,
    const char *replyType,
    const char *replyDomain,
    void *context) {

	LOG_DEBUG("browseReply: %s %s/%s as %s at if %d",
	          (flags & kDNSServiceFlagsAdd ? "Added" : "Removed"),
	          replyName,
	          replyDomain,
	          replyType,
	          ifIndex);
	shared_ptr<BonjourNodeDiscovery> myself = getInstance();
	shared_ptr<NodeQuery> query = myself->_browsers[(intptr_t)context];

	assert(myself != NULL);
	assert(query != NULL);
	assert(sdref != NULL);
	assert(replyName != NULL);
	assert(replyType != NULL);
	assert(replyDomain != NULL);

	// do we already know this node?
	shared_ptr<BonjourNodeStub> node = myself->_queryNodes[query][replyName];

	if (flags & (kDNSServiceFlagsAdd)) {
		// we have a node to add
		if (node == NULL) {
			node = shared_ptr<BonjourNodeStub>(new BonjourNodeStub());
			node->setRemote(true); /// @todo: we also find ourselves ..
			if (strncmp(replyType + strlen(replyType) - 4, "tcp", 3) == 0) {
				node->setTransport("tcp");
			} else if (strncmp(replyType + strlen(replyType) - 4, "udp", 3) == 0) {
				node->setTransport("udp");
			} else {
				LOG_WARN("Unknown transport %s, defaulting to tcp", replyType + strlen(replyType) - 4);
				node->setTransport("tcp");
			}
			node->setUUID(replyName);
			node->setDomain(replyDomain);
			// remember node
			myself->_queryNodes[query][replyName] = node;

			// add to query
			LOG_INFO("Discovered node %s:%d found", node->getUUID().c_str(), node->getPort());
			query->added(node);
		}
		node->_domains.insert(replyDomain);
		node->_interfaceIndices.insert(ifIndex);

	} else {
		// remove or change the node or an interface
		assert(node != NULL);
		node->_interfaceIndices.erase(ifIndex);
		if (node->_interfaceIndices.empty()) {
			LOG_INFO("Vanished node %s:%d", node->getUUID().c_str(), node->getPort());
			query->removed(node);
			// erase from pending changes if it get deleted anyway
			query->getPendingChanges().erase(node);
			myself->_queryNodes[query].erase(replyName);
		} else {
			LOG_INFO("Changed node %s:%d", node->getUUID().c_str(), node->getPort());
			query->changed(node);
		}
	}
	if (!(flags & kDNSServiceFlagsMoreComing)) {
		// that's it, resolve pending nodes and notify result set
		set<shared_ptr<NodeStub> >::iterator nodeIter;
		for (nodeIter = query->getPendingAdditions().begin(); nodeIter != query->getPendingAdditions().end(); nodeIter++)
			boost::static_pointer_cast<BonjourNodeStub>(*nodeIter)->resolve();
		for (nodeIter = query->getPendingChanges().begin(); nodeIter != query->getPendingChanges().end(); nodeIter++)
			boost::static_pointer_cast<BonjourNodeStub>(*nodeIter)->resolve();

		query->notifyResultSet();
	}
}

/**
 * Bonjour callback for registering a node.
 * @param sdRef The bonjour handle.
 * @param flags Only kDNSServiceFlagsAdd or not.
 * @param errorCode The error if one occured or kDNSServiceErr_NoError.
 * @param name The actual name the node succeeded to register with.
 * @param regType At the moment this ought to be _mundo._tcp only.
 * @param domain At the moment this ought to be .local only.
 * @param context The address of the node we tried to register.
 */

void DNSSD_API BonjourNodeDiscovery::registerReply(
    DNSServiceRef                       sdRef,
    DNSServiceFlags                     flags,
    DNSServiceErrorType                 errorCode,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    void                                *context
) {

	LOG_DEBUG("registerReply: %s %s/%s as %s",
	          (flags & kDNSServiceFlagsAdd ? "Registered" : "Unregistered"),
	          name,
	          domain,
	          regtype);
	if (errorCode == kDNSServiceErr_NameConflict)
		LOG_WARN("Name conflict!");

	switch (errorCode) {
	case kDNSServiceErr_NoError:
		if (flags & kDNSServiceFlagsAdd) {
			shared_ptr<NodeImpl> node = getInstance()->_nodes[(intptr_t)context];
			assert(node != NULL);
			assert(name != NULL);
			assert(domain != NULL);
			node->setUUID(name);
			node->setDomain(domain);
		} else {
			// I am not sure what it would mean to end up here
			LOG_WARN("In replyRegister from DNSServiceRegister, something fishy happened");
		}
		break;
	case kDNSServiceErr_NameConflict:
		break;
	default:
		break;
	}
	return;
}

}
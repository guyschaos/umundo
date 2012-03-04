#include "umundo/config.h"

#include "umundo/discovery/bonjour/BonjourNodeDiscovery.h"

#include <errno.h>

#if (defined UNIX || defined IOS || defined ANDROID)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#endif

#include "umundo/common/Node.h"
#include "umundo/discovery/NodeQuery.h"
#include "umundo/discovery/bonjour/BonjourNodeStub.h"

#ifdef DISC_BONJOUR_EMBED
extern "C" {
	int embedded_mDNSInit();
	void embedded_mDNSExit();
	int embedded_mDNSmainLoop(timeval);
}
#endif

namespace umundo {

BonjourNodeDiscovery::BonjourNodeDiscovery() {
	DEBUG_CTOR("BonjourNodeDiscovery");
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
#ifdef DISC_BONJOUR_EMBED
		LOG_DEBUG("Initializing embedded mDNS server");
		embedded_mDNSInit();
#endif
		_instance->start();
	}
	return _instance;
}
shared_ptr<BonjourNodeDiscovery> BonjourNodeDiscovery::_instance;

BonjourNodeDiscovery::~BonjourNodeDiscovery() {
	DEBUG_DTOR("BonjourNodeDiscovery");
#ifdef DISC_BONJOUR_EMBED
  // notify every other host that we are about to vanish
	embedded_mDNSExit();
#endif
}


void BonjourNodeDiscovery::run() {
	struct timeval tv;

#ifdef DISC_BONJOUR_EMBED
	while(isStarted()) {
		_mutex.lock();
		tv.tv_sec  = BONJOUR_REPOLL_SEC;
		tv.tv_usec = BONJOUR_REPOLL_USEC;
		embedded_mDNSmainLoop(tv);
		_mutex.unlock();
	}
#else

	fd_set readfds;
	int nfds = -1;

	LOG_DEBUG("Beginning select");
	while(isStarted()) {
		nfds = 0;
		FD_ZERO(&readfds);
		// tv structure gets modified we have to reset each time
		tv.tv_sec  = BONJOUR_REPOLL_SEC;
		tv.tv_usec = BONJOUR_REPOLL_USEC;

		std::map<int, DNSServiceRef>::const_iterator it;
		for (it = _sockFDToClients.begin(); it != _sockFDToClients.end(); it++) {
			if (it->first > nfds)
				nfds = it->first;
			FD_SET(it->first, &readfds);
		}
		nfds++;

		int result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);

		if (result > 0) {
			_mutex.lock();
			for (it = _sockFDToClients.begin(); it != _sockFDToClients.end(); it++) {
				if (FD_ISSET(it->first, &readfds)) {
					LOG_DEBUG("Processing mDNS event");
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
#endif
}

/**
 * Add a node to be discoverable by other nodes.
 */
void BonjourNodeDiscovery::add(shared_ptr<NodeImpl> node) {
	assert(node->getUUID().length() > 0);

	_mutex.lock();

	DNSServiceErrorType err;
  DNSServiceRef registerClient;
	intptr_t address = (intptr_t)(node.get());
  
	if (_localNodes.find(address) != _localNodes.end()) {
    assert(_registerClients.find(address) != _registerClients.end());
		LOG_WARN("Ignoring addition of node already added to discovery");
		_mutex.unlock();
    assert(validateState());
		return;
	}
  
  // keep in mind: we prevent the registration of the node with several BonjourNodeDiscoveries
  assert(_registerClients[address] == NULL);

	const char* name = (node->getUUID().length() ? node->getUUID().c_str() : NULL);
	const char* transport = (node->getTransport().length() ? node->getTransport().c_str() : "tcp");
	const char* host = (node->getHost().length() ? node->getHost().c_str() : NULL);
	uint16_t port = (node->getPort() > 0 ? htons(node->getPort()) : 0);
	const char* txtRecord = "";
	uint16_t txtLength = 0;

	char* regtype;
	asprintf(&regtype, "_mundo._%s", transport);

	char* domain;
	if (node->getDomain().length() > 0) {
		asprintf(&domain, "%s.local.", node->getDomain().c_str());
	} else {
		asprintf(&domain, "local.");
	}

	LOG_DEBUG("Trying to registering: %s.%s as %s",
	          (name == NULL ? "null" : name),
	          (domain == NULL ? "null" : domain),
	          regtype);

	err = DNSServiceRegister(
    &registerClient,         // uninitialized DNSServiceRef
    0,                               // renaming behavior on name conflict (kDNSServiceFlagsNoAutoRename)
    kDNSServiceInterfaceIndexAny,    // If non-zero, specifies the interface, defaults to all interfaces
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

	if(registerClient && err == 0) {
		int sockFD = DNSServiceRefSockFD(registerClient);
		getInstance()->_sockFDToClients[sockFD] = registerClient;
		getInstance()->_registerClients[address] = registerClient;
		getInstance()->_localNodes[address] = node;
	} else {
		LOG_ERR("DNSServiceRegister returned error %d", err);
	}
  assert(validateState());
	_mutex.unlock();
}

  
void BonjourNodeDiscovery::remove(shared_ptr<NodeImpl> node) {
	LOG_DEBUG("Removing node %s", node->getUUID().c_str());

  _mutex.lock();

	intptr_t address = (intptr_t)(node.get());
	if (_localNodes.find(address) == _localNodes.end()) {
		LOG_WARN("Ignoring removal of unregistered node from discovery");
    _mutex.unlock();
    assert(validateState());
		return;
	}

	if (_registerClients.find(address) != _registerClients.end()) {
    assert(_sockFDToClients.find(DNSServiceRefSockFD(_registerClients[address])) != _sockFDToClients.end());
    assert(_sockFDToClients[DNSServiceRefSockFD(_registerClients[address])] == _registerClients[address]);
    _sockFDToClients.erase(DNSServiceRefSockFD(_registerClients[address]));
    DNSServiceRefDeallocate(_registerClients[address]);
    _registerClients.erase(address);
  }

	_localNodes.erase(address);
	_mutex.unlock();
}

  
/**
 * Start to browse for other nodes.
 *
 * If we find a node we will try to resolve its hostname and if we found its hostname,
 * we will resolve its ip address.
 */
void BonjourNodeDiscovery::browse(shared_ptr<NodeQuery> query) {
	DNSServiceErrorType err;
	DNSServiceRef queryClient = NULL;

	_mutex.lock();
	intptr_t address = (intptr_t)(query.get());

	if (getInstance()->_queryClients.find(address) != getInstance()->_queryClients.end()) {
		LOG_WARN("Already browsing for given query");
		_mutex.unlock();
    assert(validateState());
		return;
	}
  assert(getInstance()->_queries.find(address) == getInstance()->_queries.end());
  
	char* regtype;
	const char* transport = (query->getTransport().length() ? query->getTransport().c_str() : "tcp");
	asprintf(&regtype, "_mundo._%s", transport);

	char* domain;
	if (query->getDomain().length() > 0) {
		asprintf(&domain, "%s.local.", query->getDomain().c_str());
	} else {
		asprintf(&domain, "local.");
	}

	err = DNSServiceBrowse(
	          &queryClient,                  // uninitialized DNSServiceRef
	          0,                             // Currently ignored, reserved for future use
	          kDNSServiceInterfaceIndexAny,  // non-zero, specifies the interface
	          regtype,                       // service type being browsed
	          domain,                        // non-NULL, specifies the domain
	          browseReply,                   // called when a service is found
	          (void*)address
	      );

	if(queryClient && err == 0) {
		int sockFD = DNSServiceRefSockFD(queryClient);
		assert(getInstance()->_queryClients[address] == NULL);
		getInstance()->_queryClients[address] = queryClient;
		getInstance()->_sockFDToClients[sockFD] = queryClient;
    getInstance()->_queries[address] = query;
	} else {
		LOG_WARN("DNSServiceBrowse returned error %d", err);
	}
	_mutex.unlock();

	free(regtype);
	free(domain);
  assert(validateState());
}

void BonjourNodeDiscovery::unbrowse(shared_ptr<NodeQuery> query) {

	_mutex.lock();
	intptr_t address = (intptr_t)(query.get());

	if (_queries.find(address) == _queries.end()) {
		LOG_WARN("Unbrowsing query that was never added");
		_mutex.unlock();
    assert(validateState());
		return;
	}

  assert(_queryClients.find(address) != _queryClients.end());
  assert(_sockFDToClients.find(DNSServiceRefSockFD(_queryClients[address])) != _sockFDToClients.end());
	assert(_sockFDToClients[DNSServiceRefSockFD(_queryClients[address])] == _queryClients[address]);
	_sockFDToClients.erase(DNSServiceRefSockFD(_queryClients[address]));
	_queryClients.erase(address);

  /** 
   * Remove all nodes we found for the query
   */
	map<intptr_t, shared_ptr<NodeQuery> >::iterator nodeToQueryIter;
  for (nodeToQueryIter = _nodeToQuery.begin(); nodeToQueryIter != _nodeToQuery.end(); nodeToQueryIter++) {
    if (nodeToQueryIter->second == query)
      _nodeToQuery.erase(nodeToQueryIter);
  }
	_queries.erase(address);

	DNSServiceRefDeallocate(_queryClients[address]);
	_mutex.unlock();
}

/**
 * There has been an answer to one of our queries.
 *
 * Gather all additions, changes and removals and start service resolving.
 */
void DNSSD_API BonjourNodeDiscovery::browseReply(
    DNSServiceRef sdref,
    const DNSServiceFlags flags,
    uint32_t ifIndex,
    DNSServiceErrorType errorCode,
    const char *replyName,
    const char *replyType,
    const char *replyDomain,
    void *queryAddr) {

	shared_ptr<BonjourNodeDiscovery> myself = getInstance();
	myself->_mutex.lock();

	LOG_DEBUG("browseReply: Received info on %s/%s as %s at if %d",
	          replyName, replyDomain, replyType, ifIndex);

  // is this actually a local node?
  map<intptr_t, shared_ptr<NodeImpl> >::iterator localNodeIter; 
  for (localNodeIter = myself->_localNodes.begin(); localNodeIter != myself->_localNodes.end(); localNodeIter++) {
    if (strcmp(replyName, localNodeIter->second->getUUID().c_str()) == 0) {
      // do nothing?
      LOG_DEBUG("Not processing local node");
      myself->_mutex.unlock();
      assert(myself->validateState());
      return;
    }
  }
  
	shared_ptr<NodeQuery> query = myself->_queries[(intptr_t)queryAddr];
	assert(query.get() != NULL);

	// do we already know this node?
	shared_ptr<BonjourNodeStub> node;
  if (getInstance()->_remoteNodes.find(replyName) != getInstance()->_remoteNodes.end())
    node = boost::static_pointer_cast<BonjourNodeStub>(getInstance()->_remoteNodes[replyName]);

	if (flags & kDNSServiceFlagsAdd) {
		// we have a node to add
		if (node.get() == NULL) {
			node = shared_ptr<BonjourNodeStub>(new BonjourNodeStub());
      // remember which query found us
      getInstance()->_nodeToQuery[(intptr_t)node.get()] = query;
			if (strncmp(replyType + strlen(replyType) - 4, "tcp", 3) == 0) {
				node->setTransport("tcp");
			} else if (strncmp(replyType + strlen(replyType) - 4, "udp", 3) == 0) {
				node->setTransport("udp");
			} else {
				LOG_WARN("Unknown transport %s, defaulting to tcp", replyType + strlen(replyType) - 4);
				node->setTransport("tcp");
			}
			node->_uuid = replyName;
			node->_domain = replyDomain;
			node->_regType = replyType;

			// remember node
      getInstance()->_remoteNodes[replyName] = node;
			LOG_INFO("Discovered new node %s:%d", node->getUUID().c_str(), node->getPort());
		}
    // even if we found the node before, add the new information
		node->_domains.insert(replyDomain);
		node->_interfaceIndices.insert(ifIndex);
	} else {
		// remove or change the node or an interface
		assert(node.get() != NULL);
		node->_interfaceIndices.erase(ifIndex);
		if (node->_interfaceIndices.empty()) {
      if (node->_isAdded) {
        // last interface was removed, node vanished
        LOG_INFO("Vanished node %s:%d", node->getUUID().c_str(), node->getPort());
        query->removed(node);

        map<string, DNSServiceRef>::iterator serviceResolveIter;
        for (serviceResolveIter = node->_serviceResolveClients.begin(); serviceResolveIter != node->_serviceResolveClients.end(); serviceResolveIter++) {
          int sockFD = DNSServiceRefSockFD(serviceResolveIter->second);
          assert(getInstance()->_sockFDToClients.find(sockFD) != getInstance()->_sockFDToClients.end());
          getInstance()->_sockFDToClients.erase(sockFD);
          DNSServiceRefDeallocate(serviceResolveIter->second);
          node->_serviceResolveClients.erase(serviceResolveIter);
        }

        map<int, DNSServiceRef>::iterator addrInfoIter;
        for (addrInfoIter = node->_addrInfoClients.begin(); addrInfoIter != node->_addrInfoClients.end(); addrInfoIter++) {
          int sockFD = DNSServiceRefSockFD(addrInfoIter->second);
          assert(getInstance()->_sockFDToClients.find(sockFD) != getInstance()->_sockFDToClients.end());
          getInstance()->_sockFDToClients.erase(sockFD);
          DNSServiceRefDeallocate(addrInfoIter->second);
          node->_addrInfoClients.erase(addrInfoIter);
        }
        assert(getInstance()->_nodeToQuery.find((intptr_t)node.get()) != getInstance()->_nodeToQuery.end());
        getInstance()->_nodeToQuery.erase((intptr_t)node.get());
				getInstance()->_remoteNodes.erase(node->getUUID());
        
      } else {
        LOG_INFO("Ancient node vanished %s:%d", node->getUUID().c_str(), node->getPort());
      }
		}
	}
	
	if (!(flags & kDNSServiceFlagsMoreComing)) {
		/**
     * There are no more bonjour notifications waiting, start resolving all new nodes
     * For each node, make sure there is a resolve client for every domain.
     */
    map<string, shared_ptr<BonjourNodeStub> >::iterator nodeIter;
		for (nodeIter = getInstance()->_remoteNodes.begin(); nodeIter != getInstance()->_remoteNodes.end(); nodeIter++) {        
      string uuid = nodeIter->first;
			shared_ptr<BonjourNodeStub> node = nodeIter->second;

			intptr_t address = (intptr_t)(node.get());
			char* regtype;
			assert(node->getTransport().length() > 0);
			asprintf(&regtype, "_mundo._%s", node->getTransport().c_str());
			const char* name = (node->getUUID().length() ? node->getUUID().c_str() : NULL);
      
      std::set<string>::iterator domainIter;
      for (domainIter = node->_domains.begin(); domainIter != node->_domains.end(); domainIter++) {
        if (node->_serviceResolveClients.find(*domainIter) != node->_serviceResolveClients.end())
          // we already have a service resolve client for that domain
          continue;

        LOG_INFO("Trying to resolve service %s.%s", node->getUUID().c_str(), domainIter->c_str());
        DNSServiceRef serviceResolveClient;
        /// Resolve service domain name, target hostname, port number and txt record
        int err = DNSServiceResolve(
          &serviceResolveClient,
          0,
          kDNSServiceInterfaceIndexAny,
          name,
          regtype,
          domainIter->c_str(),
          serviceResolveReply,
          (void*)address // address of node
        );

        if(serviceResolveClient && err == kDNSServiceErr_NoError) {
          int sockFD = DNSServiceRefSockFD(serviceResolveClient);
          node->_serviceResolveClients[*domainIter] = serviceResolveClient;
          getInstance()->_sockFDToClients[sockFD] = serviceResolveClient;
        } else {
          LOG_WARN("DNSServiceResolve returned error %d", err);
        }
      }
      
      free(regtype);
		}
	}
  assert(myself->validateState());
	myself->_mutex.unlock();
}

/**
 * We found the host for one of the services we browsed for,
 * resolve the hostname to its ip address.
 */
void DNSSD_API BonjourNodeDiscovery::serviceResolveReply(
    DNSServiceRef sdref,
    const DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *fullname,   // full service domain name: <servicename>.<protocol>.<domain>
    const char *hosttarget, // target hostname of the machine providing the service
    uint16_t opaqueport,    // port in network byte order
    uint16_t txtLen,        // length of the txt record
    const unsigned char *txtRecord, // primary txt record
    void *context           // address of node
) {

	getInstance()->_mutex.lock();
	LOG_DEBUG("serviceResolveReply: %s at %s:%d with if %d",
	          fullname,
	          hosttarget,
	          ntohs(opaqueport),
	          interfaceIndex);

	if(errorCode == kDNSServiceErr_NoError) {
    shared_ptr<NodeQuery> _query = getInstance()->_nodeToQuery[(intptr_t)context];
    shared_ptr<BonjourNodeStub> node = getInstance()->_remoteNodes[((BonjourNodeStub*)context)->getUUID()];

		node->_bonjourDomain = fullname;
		node->_hostTarget = hosttarget;
		node->_port = ntohs(opaqueport);
    
		const char* domainStart = strchr(hosttarget, '.');
		node->_domain = ++domainStart;

		char* host = (char*)malloc(strlen(hosttarget) + 1);
		memcpy(host, hosttarget, strlen(hosttarget));
		char* hostEnd = strchr(host, '.');
		hostEnd[0] = 0;
		node->_host = host;
		free(host);

    // do we remove the service resolve client now that we resolved the service?
//    assert(node->_serviceResolveClients.find(node->_domain) != node->_serviceResolveClients.end());
//    int sockFD = DNSServiceRefSockFD(node->_serviceResolveClients[node->_domain]);
//    assert(getInstance()->_sockFDToClients.find(sockFD) != getInstance()->_sockFDToClients.end());
//    getInstance()->_sockFDToClients.erase(sockFD);
//    DNSServiceRefDeallocate(node->_serviceResolveClients[node->_domain]);
//    node->_serviceResolveClients.erase(node->_domain);

    
    // I am not sure whether this is a valid assumption
    //assert(node->_addrInfoClients.find(interfaceIndex) == node->_addrInfoClients.end());
    
    DNSServiceRef addrInfoClient;

		/** Now that we know the service, resolve its IP */
		int err = DNSServiceGetAddrInfo(
			&addrInfoClient,
			kDNSServiceFlagsReturnIntermediates,     // kDNSServiceFlagsForceMulticast, kDNSServiceFlagsLongLivedQuery
			interfaceIndex,
			0,                                       // kDNSServiceProtocol_IPv4, kDNSServiceProtocol_IPv6, 0
			node->_hostTarget.c_str(),
			addrInfoReply,
			(void*)context                           // address of node
		);

		if (err == kDNSServiceErr_NoError && addrInfoClient) {
      node->_addrInfoClients[interfaceIndex] = addrInfoClient;
			int sockFD = DNSServiceRefSockFD(addrInfoClient);
			getInstance()->_sockFDToClients[sockFD] = addrInfoClient;
		} else {
			LOG_ERR("DNSServiceGetAddrInfo returned error");
		}

	} else {
		LOG_WARN("serviceResolveReply called with error: %d", errorCode);
	}
  assert(getInstance()->validateState());
	getInstance()->_mutex.unlock();
}

/**
 * We resolved the IP address for one of the nodes received in serviceResolveReply. 
 */
void DNSSD_API BonjourNodeDiscovery::addrInfoReply(
    DNSServiceRef sdRef,
    DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *hostname,
    const struct sockaddr *address,
    uint32_t ttl,
    void *context  // address of node
) {

	getInstance()->_mutex.lock();
  shared_ptr<NodeQuery> query = getInstance()->_nodeToQuery[(intptr_t)context];
  shared_ptr<BonjourNodeStub> node = getInstance()->_remoteNodes[((BonjourNodeStub*)context)->getUUID()];
  
  LOG_DEBUG("addrInfoReply: Got info on %s at if %d", hostname, interfaceIndex);

  if (node->_interfaceIndices.find(interfaceIndex) == node->_interfaceIndices.end()) {
    LOG_WARN("Got information on unknown interface index %d", interfaceIndex);
    assert(getInstance()->validateState());
    getInstance()->_mutex.unlock();
    return;
  }
  
  if (!address || (address->sa_family != AF_INET && address->sa_family != AF_INET6)) {
    LOG_ERR("Address family not valid or no address given");
    assert(getInstance()->validateState());
    getInstance()->_mutex.unlock();
    return;
  }

	switch(errorCode) {
	case kDNSServiceErr_NoError: {
		char* addr = NULL;

		if (address && address->sa_family == AF_INET) {
			const unsigned char *b = (const unsigned char *) &((struct sockaddr_in *)address)->sin_addr;
			asprintf(&addr, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
			node->_interfacesIPv4[interfaceIndex] = addr;
      LOG_DEBUG("addrInfoReply: %s as %s at if %d", hostname, addr, interfaceIndex);

      free(addr);
		}	else if (address && address->sa_family == AF_INET6) {
			const struct sockaddr_in6 *s6 = (const struct sockaddr_in6 *)address;
			const unsigned char *b = (const unsigned char*)&s6->sin6_addr;

			asprintf(&addr, "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",
			         b[0x0], b[0x1], b[0x2], b[0x3], b[0x4], b[0x5], b[0x6], b[0x7],
			         b[0x8], b[0x9], b[0xA], b[0xB], b[0xC], b[0xD], b[0xE], b[0xF]);
			node->_interfacesIPv6[interfaceIndex] = addr;
      LOG_DEBUG("addrInfoReply: %s as %s at if %d", hostname, addr, interfaceIndex);

      free(addr);
		}
		break;
	}
	case kDNSServiceErr_NoSuchRecord:
    if (address && address->sa_family == AF_INET) {
      LOG_INFO("kDNSServiceErr_NoSuchRecord for IPv4 %s at interface %d", hostname, interfaceIndex);
      node->_interfacesIPv4[interfaceIndex] = "";
    } else if (address->sa_family == AF_INET6) {
      LOG_INFO("kDNSServiceErr_NoSuchRecord for IPv6 %s at interface %d", hostname, interfaceIndex);
      node->_interfacesIPv6[interfaceIndex] = "";      
    }
		break;
	default:
      LOG_WARN("addrInfoReply error %d for %s at interface %d", errorCode, hostname, interfaceIndex);
		break;
	}
  
  // is this node fully resolved?
  if (node->_interfaceIndices.size() == node->_interfacesIPv4.size()) {
//      node->_interfaceIndices.size() == node->_interfacesIPv6.size()) {
    // we resolved all interfaces
    LOG_DEBUG("Fully resolved node %s", node->getUUID().c_str());
    if (node->_isAdded) {
      query->changed(node);
    } else {
      node->_isAdded = true;
      query->added(node);          
    }
  }

  assert(getInstance()->validateState());
	getInstance()->_mutex.unlock();
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
    DNSServiceRef            sdRef,
    DNSServiceFlags          flags,
    DNSServiceErrorType      errorCode,
    const char               *name,
    const char               *regtype,
    const char               *domain,
    void                     *context // address of node
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
      shared_ptr<NodeImpl> node = getInstance()->_localNodes[(intptr_t)context];
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
  assert(getInstance()->validateState());
  return;
}

bool BonjourNodeDiscovery::validateState() {
  _mutex.lock();
  map<int, DNSServiceRef>::iterator sockFDToClientIter;
  map<intptr_t, shared_ptr<NodeImpl> >::iterator localNodeIter;
  map<intptr_t, DNSServiceRef>::iterator registerClientIter;
  map<intptr_t, shared_ptr<NodeQuery> >::iterator queryIter;
  map<intptr_t, DNSServiceRef>::iterator queryClientIter;
  map<intptr_t, shared_ptr<NodeQuery> >::iterator nodeToQueryIter;
  map<string, shared_ptr<BonjourNodeStub> >::iterator remoteNodeIter;
  
  map<string, DNSServiceRef>::iterator serviceResolveClientIter;
  map<int, DNSServiceRef>::iterator addrInfoClientIter;
  
  set<int> socketFDs;
  // make sure socket file descriptors are consistent - noop in embedded
#ifndef DISC_BONJOUR_EMBED
  for (sockFDToClientIter = _sockFDToClients.begin(); sockFDToClientIter != _sockFDToClients.end(); sockFDToClientIter++) {
    // key is file descriptor of bonjour handle
    assert(sockFDToClientIter->first == DNSServiceRefSockFD(sockFDToClientIter->second));
    // gather all socket fds to eliminate them in subsequent tests
    socketFDs.insert(sockFDToClientIter->first);
  }
#endif

  // test something about local nodes?
  
  // test register clients
  for (registerClientIter = _registerClients.begin(); registerClientIter != _registerClients.end(); registerClientIter++) {
    // we have this register query's socket in the set of all sockets
#ifndef DISC_BONJOUR_EMBED
    assert(socketFDs.find(DNSServiceRefSockFD(registerClientIter->second)) != socketFDs.end());
    socketFDs.erase(DNSServiceRefSockFD(registerClientIter->second));
#endif
    // the is a node to the register query
    assert(_localNodes.find(registerClientIter->first) != _localNodes.end());
  }

  for (queryIter = _queries.begin(); queryIter != _queries.end(); queryIter++) {
    // we have a bonjour query running for this query
    assert(_queryClients.find(queryIter->first) != _queryClients.end());
    // we have this queries socket in all sockets
#ifndef DISC_BONJOUR_EMBED
    assert(socketFDs.find(DNSServiceRefSockFD(_queryClients[queryIter->first])) != socketFDs.end());
    socketFDs.erase(DNSServiceRefSockFD(_queryClients[queryIter->first]));
#endif
  }
  // there are as many queries as there are query clients in bonjour
  assert(_queryClients.size() == _queries.size());
  
  for (remoteNodeIter = _remoteNodes.begin(); remoteNodeIter != _remoteNodes.end(); remoteNodeIter++) {
    assert(remoteNodeIter->first.compare(remoteNodeIter->second->getUUID()) == 0);
    for (serviceResolveClientIter = remoteNodeIter->second->_serviceResolveClients.begin(); serviceResolveClientIter != remoteNodeIter->second->_serviceResolveClients.end(); serviceResolveClientIter++) {
#ifndef DISC_BONJOUR_EMBED
      assert(socketFDs.find(DNSServiceRefSockFD(serviceResolveClientIter->second)) != socketFDs.end());
      socketFDs.erase(DNSServiceRefSockFD(serviceResolveClientIter->second));
#endif
    }
    for (addrInfoClientIter = remoteNodeIter->second->_addrInfoClients.begin(); addrInfoClientIter != remoteNodeIter->second->_addrInfoClients.end(); addrInfoClientIter++) {
#ifndef DISC_BONJOUR_EMBED
      assert(socketFDs.find(DNSServiceRefSockFD(addrInfoClientIter->second)) != socketFDs.end());
      socketFDs.erase(DNSServiceRefSockFD(addrInfoClientIter->second));
#endif
    }
  }
  
#ifndef DISC_BONJOUR_EMBED
  assert(socketFDs.size() == 0);
#endif

  LOG_DEBUG("Validated state: %d remote nodes, %d local nodes, %d queries", _remoteNodes.size(), _localNodes.size(), _sockFDToClients.size());
  _mutex.unlock();
  return true;
}

}

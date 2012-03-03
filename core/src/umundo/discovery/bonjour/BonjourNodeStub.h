#ifndef BonjourNodeStub_H_WRJ8277D
#define BonjourNodeStub_H_WRJ8277D

#include "dns_sd.h"

#include "umundo/common/NodeStub.h"
#include "umundo/thread/Thread.h"

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
	bool _isAdded;
	map<int, string> _interfacesIPv4;
	map<int, string> _interfacesIPv6;

	std::set<string> _domains;
	std::set<int> _interfaceIndices;
	string _regType;
	string _bonjourDomain;
	string _hostTarget;

	map<string, DNSServiceRef> _serviceResolveClients; ///< Bonjour handle for node/service resolving per domain.
	map<int, DNSServiceRef> _addrInfoClients; ///< Bonjour handle for address info per interface.

	friend std::ostream& operator<<(std::ostream&, const BonjourNodeStub*);
	friend class BonjourNodeDiscovery;
};

}

#endif /* end of include guard: BonjourNodeStub_H_WRJ8277D */

#ifndef AVAHINODESTUB_H_ACCARM71
#define AVAHINODESTUB_H_ACCARM71

#include "common/NodeStub.h"

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-client/lookup.h>

#include <sys/socket.h>
#include <netinet/in.h>

namespace umundo {

class AvahiNodeStubDiscovery;

/**
 * Concrete nodestub implementor for avahi (bridge pattern).
 */
class AvahiNodeStub : public NodeStub {
public:
	AvahiNodeStub();
	virtual ~AvahiNodeStub();

	uint16_t getPort();
	const string& getDomain();
	const string& getHost();
	const string& getIP();

private:
	void resolve();

	bool _isOurOwn;
	bool _isWan;
	map<int, string> _interfaces;
	string _txtRecord;

	friend std::ostream& operator<<(std::ostream&, const AvahiNodeStub*);
	friend class AvahiNodeDiscovery;
};

}

#endif /* end of include guard: AVAHINODESTUB_H_ACCARM71 */

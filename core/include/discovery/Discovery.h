#ifndef DISCOVERY_H_PWR3M1QA
#define DISCOVERY_H_PWR3M1QA

#include "common/Node.h"
#include "discovery/NodeQuery.h"

namespace umundo {

class DiscoveryImpl {
public:
	virtual DiscoveryImpl* create() = 0;

	virtual void remove(shared_ptr<Node> node) = 0;
	virtual void add(shared_ptr<Node> node) = 0;

	virtual void browse(NodeQuery* discovery) = 0;
	virtual void unbrowse(NodeQuery* discovery) = 0;

};

class Discovery {
public:
	Discovery();
	~Discovery();

	static void remove(shared_ptr<Node> node);
	static void add(shared_ptr<Node> node);

	static void browse(NodeQuery* discovery);
	static void unbrowse(NodeQuery* discovery);

protected:
	DiscoveryImpl* _impl;
};

}

#endif /* end of include guard: DISCOVERY_H_PWR3M1QA */

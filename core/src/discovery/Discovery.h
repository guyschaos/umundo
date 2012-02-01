#ifndef DISCOVERY_H_PWR3M1QA
#define DISCOVERY_H_PWR3M1QA

#include "common/Node.h"
#include "discovery/NodeQuery.h"

namespace umundo {

/**
 * Discovery implementor basis class (bridge pattern).
 * \see Discovery
 */
class DiscoveryImpl {
public:
	/// Factory method to create instances from a prototype instance.
	virtual DiscoveryImpl* create() = 0;

	virtual void add(shared_ptr<Node> node) = 0;
	virtual void remove(shared_ptr<Node> node) = 0;

	virtual void browse(NodeQuery* discovery) = 0;
	virtual void unbrowse(NodeQuery* discovery) = 0;

};

/**
 * Abstraction of the discovery subsystem (bridge pattern).
 *
 * Concrete implementors of this class provide advertising and discovery of remote and local nodes within a domain.
 * 
 * - Enable remote Discovery implementors to find locally added Nodes as NodeStub%s.
 * - Allow the application to browse for Node%s via NodeQuery%s.
 *
 * \see DiscoveryImpl defines the abstract base class for concrete implementors.
 */
class Discovery {
public:
	/**
	 * Create a new discovery subsystem.
	 *
	 * This will call the Factory to instantiate a new concrete implementor from its registered prototype.
	 */
	Discovery(); ///< Create a new discovery subsystem
	~Discovery();

	/** @name Management of local nodes */
  //@{
	static void add(shared_ptr<Node> node);    ///< Add a Node to multicast domain discovery.
	static void remove(shared_ptr<Node> node); ///< Remove a Node from multicast domain disc.
  //@}

	/** @name Query for nodes */
  //@{
	static void browse(NodeQuery* discovery);
	static void unbrowse(NodeQuery* discovery);
  //@}

protected:
	DiscoveryImpl* _impl; ///< The concrete implementor instance.
};

}

#endif /* end of include guard: DISCOVERY_H_PWR3M1QA */

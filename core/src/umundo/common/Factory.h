#ifndef FACTORY_H_BRZEE6H
#define FACTORY_H_BRZEE6H

#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"

namespace umundo {

class Implementation;
class Configuration;

/**
 * Creates instances of implementations for subsystems at runtime (factory pattern).
 *
 * This class realizes the Factory pattern by instantiating objects form prototypes. If you want to implement a specific subsystem
 * yourself, just inherit its base-class and register a prototype at the factory.
 *
 * \see PublisherImpl, SubscriberImpl, DiscoveryImpl, NodeImpl
 */
class Factory {
public:
	static Factory* getInstance();
	static shared_ptr<Implementation> create(string, void* facade = NULL);
	static shared_ptr<Configuration> config(string);

	static void suspendInstances(); ///< Suspend all instances for device sleep
	static void resumeInstances(); ///< Resume all instances from device sleep

	static void registerPrototype(string, Implementation*, Configuration*);

protected:
	Factory();

private:
	map<string, Implementation*> _prototypes;
	map<string, Configuration*> _configures;
	vector<weak_ptr<Implementation> > _implementations;
	Mutex _mutex;
	static Factory* _instance;

};

}

#endif /* end of include guard: FACTORY_H_BRZEE6H */

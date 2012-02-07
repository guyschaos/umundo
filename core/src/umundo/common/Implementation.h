#ifndef IMPLEMENTATION_H_NZ2M9TXJ
#define IMPLEMENTATION_H_NZ2M9TXJ

#include "umundo/common/Common.h"

namespace umundo {
	class Configuration;
	/**
	 * Abstract base class for concrete implementations (bridge pattern).
	 *
	 * Concrete implementors are registered at program initialization at the Factory and
	 * instantiated for every Abstraction that needs an implementation.
	 */
	class Implementation {
	public:
		virtual void destroy() = 0; ///< Delegate destruction to the implementation
		virtual void init(shared_ptr<Configuration>) = 0; ///< initialize instance after creation
	private:
		virtual shared_ptr<Implementation> create() = 0; ///< Factory method called by the Factory class
		friend class Factory; ///< In C++ friends can see your privates!
	};
	
	/**
	 * Abstract base class for configuration of Implementation%s.
	 *
	 * C++ does not allow Implementation::init() to take non-PODs as a variadic function, we need
	 * some way to abstract configuration data. We could also pass it to Factory::create(), but I
	 * like the idea of a dedicated init phase.
	 *
	 * \todo The Configuration is shared between the Abstraction and the concrete Implementor - we might need a mutex.
	 */
	class Configuration {
	private:
		virtual shared_ptr<Configuration> create() = 0;
		friend class Factory;
	};
}

#endif /* end of include guard: IMPLEMENTATION_H_NZ2M9TXJ */

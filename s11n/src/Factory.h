#ifndef FACTORY_H_C27ZUCZH
#define FACTORY_H_C27ZUCZH

#include "config.h"
#include "TypedPublisher.h"
#include "TypedSubscriber.h"

#ifdef S11N_PROTOBUF
#include "protobuf/PBSerializer.h"
#include "protobuf/PBTypedSubscriber.h"
#endif

namespace umundo {
	
	class S11NFactory {
	public:
		static S11NFactory* getInstance();
		static TypeSerializerImpl* createTypeSerializer();

	protected:
		S11NFactory();

		TypeSerializerImpl* _typedSerializerImpl;

	private:
		static S11NFactory* _instance;

	};
	
}

#endif /* end of include guard: FACTORY_H_C27ZUCZH */

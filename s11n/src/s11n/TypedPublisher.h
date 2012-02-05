#ifndef TYPEDPUBLISHER_H_9RTI6TXT
#define TYPEDPUBLISHER_H_9RTI6TXT

#include "connection/Publisher.h"
#include "common/Implementation.h"

namespace umundo {
	
	class TypeSerializerImpl : public Implementation {
	public:
		virtual string serialize(const string& type, void* obj) = 0;
		virtual void registerType(const string& type, void* serializer) = 0;
	};
	
	class TypedPublisher : public Publisher {
	public:
		TypedPublisher(string channelName);
		virtual ~TypedPublisher();
		void sendObj(const string& type, void* obj);
		void registerType(const string& type, void* serializer);
	private:
		shared_ptr<TypeSerializerImpl> _impl;
		
		static TypeSerializerImpl* _registeredPrototype; ///< The instance we registered at the factory
	};
	
}

#endif /* end of include guard: TYPEDPUBLISHER_H_9RTI6TXT */

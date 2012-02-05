#ifndef TYPEDSUBSCRIBER_H_ASH7AO4U
#define TYPEDSUBSCRIBER_H_ASH7AO4U

#include "connection/Subscriber.h"
#include "common/Implementation.h"

namespace umundo {
	
	class TypedReceiver {
	public:
		virtual void receive(void* object, Message* msg) = 0;
	};
	
	class TypeDeserializerImpl : public Implementation {
	public:
		virtual void* deserialize(const string& type, const string& data) = 0;
		virtual void registerType(const string& type, void* deserializer) = 0;
	};
	
	class TypedSubscriber : public Subscriber, public Receiver {
	public:
		TypedSubscriber(string channelName, TypedReceiver* recv);
		virtual ~TypedSubscriber();
		void registerType(const string& type, void* serializer);
		void receive(Message* msg);

	private:
		shared_ptr<TypeDeserializerImpl> _impl;
		TypedReceiver* _recv;
		static TypeDeserializerImpl* _registeredPrototype; ///< The instance we registered at the factory
	};
	
}

#endif /* end of include guard: TYPEDSUBSCRIBER_H_ASH7AO4U */

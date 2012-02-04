#ifndef TYPEDSUBSCRIBER_H_5WOOONH3
#define TYPEDSUBSCRIBER_H_5WOOONH3

#include "connection/Subscriber.h"

namespace umundo {
	
	class TypedSubscriberImpl : public SubscriberImpl {
	public:
		virtual TypedSubscriberImpl* create() = 0;
		virtual void destroy() = 0;
		virtual void registerDeserializer(string type, void* serializer) = 0;
	};
	
	class TypedSubscriber : public Subscriber {
	public:
		TypedSubscriber(string channelName);
		virtual ~TypedSubscriber();
		void sendObj(const string& type, void* obj);
		void registerDeserializer(string type, void* serializer);
	private:
		TypedSubscriberImpl* _impl;
	};
	
}

#endif /* end of include guard: TYPEDSUBSCRIBER_H_5WOOONH3 */

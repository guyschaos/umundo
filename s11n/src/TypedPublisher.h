#ifndef TYPEDPUBLISHER_H_9RTI6TXT
#define TYPEDPUBLISHER_H_9RTI6TXT

#include "connection/Publisher.h"

namespace umundo {
	
	class TypeSerializerImpl {
	public:
		virtual TypeSerializerImpl* create() = 0;
		virtual void destroy() = 0;

		virtual string serialize(const string& type, void* obj) = 0;
		virtual void registerType(const string& type, void* serializer) = 0;
	};
	
	class TypedPublisher : public umundo::Publisher {
	public:
		TypedPublisher(string channelName);
		virtual ~TypedPublisher();
		void sendObj(const string& type, void* obj);
		void registerType(const string& type, void* serializer);
	private:
		TypeSerializerImpl* _serializer;
	};
	
}

#endif /* end of include guard: TYPEDPUBLISHER_H_9RTI6TXT */

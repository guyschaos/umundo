#ifndef PBTYPEDPUBLISHER_H_XJ5CK2XS
#define PBTYPEDPUBLISHER_H_XJ5CK2XS

#include "../TypedPublisher.h"
#include <google/protobuf/message_lite.h>

using google::protobuf::MessageLite;

namespace umundo {
	
	class PBSerializer : public TypeSerializerImpl {
	public:
		PBSerializer();
		virtual ~PBSerializer();
		
		virtual TypeSerializerImpl* create();
		virtual void destroy();

		virtual string serialize(const string& type, void* obj);
		virtual void registerType(const string& type, void* serializer);
		
	private:
		map<string, MessageLite*> _serializers;
	};
	
}

#endif /* end of include guard: PBTYPEDPUBLISHER_H_XJ5CK2XS */

#ifndef PBSERIALIZER_H_LQKL8UQG
#define PBSERIALIZER_H_LQKL8UQG

#include "umundo/s11n/TypedPublisher.h"
#include <google/protobuf/message_lite.h>

using google::protobuf::MessageLite;

namespace umundo {

class PBSerializer : public TypeSerializerImpl {
public:
	PBSerializer();
	virtual ~PBSerializer();

	virtual shared_ptr<Implementation> create(void*);
	virtual void destroy();
	virtual void init(shared_ptr<Configuration>);

	virtual string serialize(const string& type, void* obj);
//		virtual string serialize(void* obj);
	virtual void registerType(const string& type, void* serializer);

private:
	map<string, MessageLite*> _serializers;
};

}

#endif /* end of include guard: PBSERIALIZER_H_LQKL8UQG */

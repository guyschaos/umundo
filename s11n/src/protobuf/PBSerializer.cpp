#include "PBSerializer.h"

namespace umundo {
PBSerializer::PBSerializer() {}

PBSerializer::~PBSerializer() {}

TypeSerializerImpl* PBSerializer::create() {
	return new PBSerializer();
}

void PBSerializer::destroy() {
	
}

string PBSerializer::serialize(const string& type, void* obj) {
  MessageLite* pbObj = (MessageLite*)obj;
  return pbObj->SerializeAsString();
}

void PBSerializer::registerType(const string& type, void* serializer) {
	_serializers[type] = (MessageLite*)serializer;
}
	
	
}
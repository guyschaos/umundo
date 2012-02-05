#include "PBSerializer.h"
#include "common/Factory.h"

namespace umundo {
	
PBSerializer::PBSerializer() {}

PBSerializer::~PBSerializer() {}

shared_ptr<Implementation> PBSerializer::create() {
	shared_ptr<Implementation> instance(new PBSerializer());
	return instance;
}

void PBSerializer::destroy() {
	
}

void PBSerializer::init(shared_ptr<Configuration> config) {
}

string PBSerializer::serialize(const string& type, void* obj) {
  MessageLite* pbObj = (MessageLite*)obj;
  return pbObj->SerializeAsString();
}

void PBSerializer::registerType(const string& type, void* serializer) {
	_serializers[type] = (MessageLite*)serializer;
}

}
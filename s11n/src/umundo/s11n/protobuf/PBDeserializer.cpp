#include "umundo/s11n/protobuf/PBDeserializer.h"

namespace umundo {

void PBDeserializer::destroy() {}

void PBDeserializer::init(shared_ptr<Configuration>) {}

shared_ptr<Implementation> PBDeserializer::create() {
	shared_ptr<Implementation> instance(new PBDeserializer());
	return instance;
}

// void* PBDeserializer::deserialize(const string& data) {
// 	MessageLite* pbObj = new PBTypedMessage();
// 	pbObj->ParseFromString(data);
// 	// \todo: unpack eventual custom type
// 	return pbObj;
// }

void* PBDeserializer::deserialize(const string& type, const string& data) {
  if (_deserializers.find(type) == _deserializers.end()) {
    LOG_ERR("received type %s, but no deserializer is known", type.c_str());
    return NULL;
  }
	MessageLite* pbObj = _deserializers[type]->New();
	pbObj->ParseFromString(data);
	return pbObj;
}

void PBDeserializer::registerType(const std::string& type, void* deserializer) {
  _deserializers[type] = (MessageLite*)deserializer;
}
	
}
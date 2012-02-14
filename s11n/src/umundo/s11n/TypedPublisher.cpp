#include "umundo/s11n/TypedPublisher.h"
#include "umundo/common/Factory.h"
#include "umundo/common/Message.h"

#include "umundo/config.h"
#ifdef S11N_PROTOBUF
#include "umundo/s11n/protobuf/PBSerializer.h"
#else
#error No serialization implementation choosen
#endif

namespace umundo {

TypedPublisher::TypedPublisher(string channelName) : Publisher(channelName) {
	if (_registeredPrototype == NULL) {
#ifdef S11N_PROTOBUF
		_registeredPrototype = new PBSerializer();
#endif
		Factory::registerPrototype("typeSerializer", _registeredPrototype, NULL);
	}
	_impl = boost::static_pointer_cast<TypeSerializerImpl>(Factory::create("typeSerializer"));
	assert(_impl != NULL);
}
TypeSerializerImpl* TypedPublisher::_registeredPrototype = NULL;

TypedPublisher::~TypedPublisher() {
}

void TypedPublisher::sendObj(const string& type, void* obj) {
	Message* msg = new Message();
	string buffer(_impl->serialize(type, obj));
	msg->setData(buffer);
	msg->setMeta("type", type);
	send(msg);
}

// void TypedPublisher::sendObj(void* obj) {
// 	Message* msg = new Message();
// 	string buffer(_impl->serialize(obj));
// 	msg->setData(buffer);
// 	send(msg);
// }

void TypedPublisher::registerType(const string& type, void* serializer) {
	_impl->registerType(type, serializer);	
}

}

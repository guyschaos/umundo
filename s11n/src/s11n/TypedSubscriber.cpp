#include "TypedSubscriber.h"
#include "common/Factory.h"
#include "common/Message.h"

#include "config.h"
#ifdef S11N_PROTOBUF
#include "protobuf/PBDeserializer.h"
#else
#error No serialization implementation choosen
#endif

namespace umundo {

TypedSubscriber::TypedSubscriber(string channelName, TypedReceiver* recv) : Subscriber(channelName, this) {
	if (_registeredPrototype == NULL) {
#ifdef S11N_PROTOBUF
		_registeredPrototype = new PBDeserializer();
#endif
		Factory::registerPrototype("typeDeserializer", _registeredPrototype, NULL);
	}
	_recv = recv;
	_impl = boost::static_pointer_cast<TypeDeserializerImpl>(Factory::create("typeDeserializer"));
	assert(_impl != NULL);
}
TypeDeserializerImpl* TypedSubscriber::_registeredPrototype = NULL;

TypedSubscriber::~TypedSubscriber() {
}

void TypedSubscriber::registerType(const string& type, void* deserializer) {
	_impl->registerType(type, deserializer);	
}

void TypedSubscriber::receive(Message* msg) {
	_recv->receive(_impl->deserialize(msg->getMeta("type"), msg->getData()), msg);
}

}

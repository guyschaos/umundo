#include "TypedSubscriber.h"
#include "protobuf/PBDeserializer.h"
#include "common/Factory.h"

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

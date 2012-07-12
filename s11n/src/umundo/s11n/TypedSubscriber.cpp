/**
 *  Copyright (C) 2012  Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 */

#include "umundo/s11n/TypedSubscriber.h"
#include "umundo/common/Factory.h"
#include "umundo/common/Message.h"

#include "umundo/config.h"
#ifdef S11N_PROTOBUF
#include "umundo/s11n/protobuf/PBDeserializer.h"
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
	if (msg->getMeta().find("type") != msg->getMeta().end()) {
		// explicit type given
		void* obj = _impl->deserialize(msg->getMeta("type"), string(msg->data(), msg->size()));
		_recv->receive(obj, msg);
//    _impl->destroyObj(obj);
	} else {
		// self describing message
		//_recv->receive(_impl->deserialize(msg->getData()), msg);
	}
}

}

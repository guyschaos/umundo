#ifndef TYPEDPUBLISHER_CPP_1O1I6YN9
#define TYPEDPUBLISHER_CPP_1O1I6YN9

#include "TypedPublisher.h"
#include "Factory.h"

namespace umundo {

TypedPublisher::TypedPublisher(string channelName) : Publisher(channelName) {
	_serializer = S11NFactory::createTypeSerializer();
	assert(_serializer != NULL);
}

TypedPublisher::~TypedPublisher() {
	_serializer->destroy();
	_serializer = NULL;
}

void TypedPublisher::sendObj(const string& type, void* obj) {
	string buffer(_serializer->serialize(type, obj));
	send(buffer.data(), buffer.size());
}

void TypedPublisher::registerType(const string& type, void* serializer) {
	_serializer->registerType(type, serializer);	
}

}

#endif /* end of include guard: TYPEDPUBLISHER_CPP_1O1I6YN9 */

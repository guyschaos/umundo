#include "Factory.h"

namespace umundo {

S11NFactory* S11NFactory::_instance = NULL;

S11NFactory* S11NFactory::getInstance() {
	if (_instance == NULL) {
		_instance = new S11NFactory();
		assert(_instance->_typedSerializerImpl != NULL);
	}
	return _instance;
}

S11NFactory::S11NFactory() {
#ifdef S11N_PROTOBUF	
	_typedSerializerImpl = new PBSerializer();
#endif
}

TypeSerializerImpl* S11NFactory::createTypeSerializer() {
	return getInstance()->_typedSerializerImpl->create();
}

}
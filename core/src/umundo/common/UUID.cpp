#include "umundo/common/UUID.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace umundo {

boost::uuids::random_generator UUID::randomGen;
const string UUID::getUUID() {
	return boost::lexical_cast<string>(randomGen());
}

}

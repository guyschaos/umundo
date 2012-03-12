#ifndef UUID_H_ASB7D2U4
#define UUID_H_ASB7D2U4

#include "umundo/common/Common.h"
#include <boost/uuid/uuid_generators.hpp>

namespace umundo {
	
class UUID {
public:
	static const string getUUID();

private:
	static boost::uuids::random_generator randomGen;
};

}

#endif /* end of include guard: UUID_H_ASB7D2U4 */

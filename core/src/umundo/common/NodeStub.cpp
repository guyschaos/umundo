#include "umundo/common/NodeStub.h"

namespace umundo {

std::ostream& operator<<(std::ostream &out, const NodeStub* n) {
	out << SHORT_UUID(n->getUUID()) << ": ";
	out << n->getHost() << "";
	out << n->getDomain() << ":";
	out << n->getPort();

	// map<int, string>::const_iterator iFaceIter;
	// for (iFaceIter = n->_interfaces.begin(); iFaceIter != n->_interfaces.end(); iFaceIter++) {
	// 	out << "\t" << iFaceIter->first << ": " << iFaceIter->second << std::endl;
	// }
	return out;
}

}
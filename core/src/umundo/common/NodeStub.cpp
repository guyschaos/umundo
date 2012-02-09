#include "umundo/common/NodeStub.h"

namespace umundo {
  
std::ostream& operator<<(std::ostream &out, const NodeStub* n) {
	out << std::endl;
	out << n->getUUID() << ": ";
	out << n->getHost() << ".";
	out << n->getDomain() << ":";
	out << n->getPort() << std::endl;

	// map<int, string>::const_iterator iFaceIter;
	// for (iFaceIter = n->_interfaces.begin(); iFaceIter != n->_interfaces.end(); iFaceIter++) {
	// 	out << "\t" << iFaceIter->first << ": " << iFaceIter->second << std::endl;
	// }
	return out;
}

}
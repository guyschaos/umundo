#ifndef NODE_H_AA94X8L6
#define NODE_H_AA94X8L6

#include <boost/lexical_cast.hpp>

#include "common/stdInc.h"

#include "common/NodeStub.h"

namespace umundo {

/**
 * The local umundo node implementor basis class (bridge pattern).
 */
class Node : public NodeStub {
public:
	Node() {
		_uuid = boost::lexical_cast<string>(boost::uuids::random_generator()());
	}

protected:
//	NodeImpl* _impl;
};

}


#endif /* end of include guard: NODE_H_AA94X8L6 */

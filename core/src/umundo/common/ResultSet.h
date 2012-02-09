#ifndef RESULTSET_H_D2O6OBDA
#define RESULTSET_H_D2O6OBDA

#include "umundo/common/Common.h"

namespace umundo {

template<class T>

/**
 * Interface to be notified about addition, removal or changes of entities.
 */
class ResultSet {
public:
	virtual void added(shared_ptr<T>) = 0;
	virtual void removed(shared_ptr<T>) = 0;
	virtual void changed(shared_ptr<T>) = 0;
};
}

#endif /* end of include guard: RESULTSET_H_D2O6OBDA */
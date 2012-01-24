#ifndef RESULTSET_H_D2O6OBDA
#define RESULTSET_H_D2O6OBDA

namespace umundo {

template<class T>

class ResultSet {
public:
	virtual void added(shared_ptr<T>) = 0;
	virtual void removed(shared_ptr<T>) = 0;
	virtual void changed(shared_ptr<T>) = 0;
};
}

#endif /* end of include guard: RESULTSET_H_D2O6OBDA */

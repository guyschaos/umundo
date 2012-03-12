#ifndef NODESTUB_H_20YUUS31
#define NODESTUB_H_20YUUS31

#include "umundo/common/Common.h"
#include "umundo/common/EndPoint.h"

#define SHORT_UUID(uuid) uuid.substr(0, 8)

namespace umundo {

/**
 * Representation of a remote umundo Node.
 */
class NodeStub : public EndPoint {
public:
	virtual ~NodeStub() {}

	/** @name Remote Node */
	//@{
	virtual const string& getUUID() const       {
		return _uuid;
	}
	virtual void setUUID(string uuid)           {
		_uuid = uuid;
	}
	//@}

	inline bool operator==(NodeStub* n) const {
		return (getUUID().compare(n->getUUID()) == 0);
	}

	inline bool operator!=(NodeStub* n) const {
		return (getUUID().compare(n->getUUID()) != 0);
	}

protected:
	string _uuid;

private:
	friend std::ostream& operator<<(std::ostream&, const NodeStub*);

};
}

#endif /* end of include guard: NODESTUB_H_20YUUS31 */

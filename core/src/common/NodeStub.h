#ifndef NODESTUB_H_20YUUS31
#define NODESTUB_H_20YUUS31

#include "config.h"
#include "common/stdInc.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "common/EndPoint.h"

namespace umundo {

/**
 * Representation of a remote umundo Node.
 */
class NodeStub : public EndPoint, public boost::enable_shared_from_this<NodeStub> {
public:
	/**
	 * Universal Unique ID
	 */
	virtual const string& getUUID() {
		return _uuid;
	}
	virtual void setUUID(string uuid) {
		_uuid = uuid;
	}

	inline bool operator==(NodeStub n) const {
		return (_uuid.compare(n._uuid) == 0);
	}

	inline bool operator!=(NodeStub n) const {
		return (_uuid.compare(n._uuid) != 0);
	}
protected:
	std::string _uuid;
	bool _isRemote;
	bool _isAdded;
	string _txtRecord;
	map<int, string> _interfaces;               // interfaces to ip addresses

private:
	friend std::ostream& operator<<(std::ostream&, const NodeStub*);

};
}

#endif /* end of include guard: NODESTUB_H_20YUUS31 */

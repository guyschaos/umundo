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
class NodeStub : public EndPoint {
public:
	/** @name Remote Node */
	//@{
	virtual const string& getUUID() const       { return _uuid; }
	virtual void setUUID(string uuid)           { _uuid = uuid; }
	virtual const string& getHost() const       { return _host; }
	virtual void setHost(string host)           { _host = host; }
	virtual const string& getDomain() const     { return _domain; }
	virtual void setDomain(string domain)       { _domain = domain; }
	//@}

	inline bool operator==(NodeStub* n) const {
		return (getUUID().compare(n->getUUID()) == 0);
	}

	inline bool operator!=(NodeStub* n) const {
		return (getUUID().compare(n->getUUID()) != 0);
	}

protected:
	string _uuid;
	string _host;
	string _domain;

private:
	friend std::ostream& operator<<(std::ostream&, const NodeStub*);

};
}

#endif /* end of include guard: NODESTUB_H_20YUUS31 */

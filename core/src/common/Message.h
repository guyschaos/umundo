#ifndef MESSAGE_H_Y7TB6U8
#define MESSAGE_H_Y7TB6U8

#include "config.h"
#include "common/stdInc.h"

#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace umundo {
class Message;
class Type;

/**
 * Definition of message types and abstraction of message - not used (bridge pattern).
 *
 * We are still pondering whether we need an explicit message representation. At the moment, only the Type enum is used.
 */
class Message {
public:
	enum Type {
	    DATA          = 0x0000,
	    PUB_ADDED     = 0x0004,
	    PUB_REMOVED   = 0x0005,
	    NODE_INFO     = 0x0006,
	};

	Message() {}
	Message(string data) : _data(data) {}
	Message(const char* data, size_t length) : _data(data, length) {}
	virtual ~Message() {}

	virtual const string& getData()                                     { return _data; }
	virtual void setData(const string& data)                            { _data = data; }
	virtual const void setMeta(const string& key, const string& value)  { _meta[key] = value; }
	virtual const map<string, string>& getMeta()                        { return _meta; }
	virtual const string& getMeta(const string& key)                    { return _meta[key]; }

protected:
	string _data;
	map<string, string> _meta;
};
}


#endif /* end of include guard: MESSAGE_H_Y7TB6U8 */

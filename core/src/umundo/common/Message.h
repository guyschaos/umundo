#ifndef MESSAGE_H_Y7TB6U8
#define MESSAGE_H_Y7TB6U8

#include "umundo/common/Common.h"

namespace umundo {
class Message;
class Type;

/**
 * Definition of message types and abstraction of message (bridge pattern).
 */
class Message {
public:
	enum Type {
	    DATA          = 0x0000,
	    PUB_ADDED     = 0x0004,
	    PUB_REMOVED   = 0x0005,
	    NODE_INFO     = 0x0006,
      SUBSCRIBE     = 0x0007,
      UNSUBSCRIBE   = 0x0008,
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
	/// Simplified access to keyset for Java, namespace qualifiers required for swig!
	virtual const std::vector<std::string>& getKeys() {
		map<string, string>::const_iterator metaIter;
		_keys.clear();
		for (metaIter = _meta.begin(); metaIter != _meta.end(); metaIter++) {
			_keys.push_back(metaIter->first);
		}
		return _keys;
	}

protected:
	string _data;
	map<string, string> _meta;
	vector<string> _keys;
};
}


#endif /* end of include guard: MESSAGE_H_Y7TB6U8 */

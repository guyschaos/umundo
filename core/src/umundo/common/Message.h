#ifndef MESSAGE_H_Y7TB6U8
#define MESSAGE_H_Y7TB6U8

#include "umundo/common/Common.h"

namespace umundo {
class Message;
class Type;

class MemoryBuffer {
public:
	char* data;
	size_t size;
};

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

	static const char* typeToString(uint16_t type) {
		if (type == 0x0000) return "DATA";
		if (type == 0x0004) return "PUB_ADDED";
		if (type == 0x0005) return "PUB_REMOVED";
		if (type == 0x0006) return "NODE_INFO";
		if (type == 0x0007) return "SUBSCRIBE";
		if (type == 0x0008) return "UNSUBSCRIBE";
		return "UNKNOWN";
	}

	Message() {}
	Message(string data) : _data(data) {}
	Message(const char* data, size_t length) : _data(data, length) {}
	virtual ~Message() {}

	virtual const string& getData()                                     {
		return _data;
	}
	virtual void setData(const string& data)                            {
		_data = data;
	}
	virtual void setData(const char* data, size_t length)               {
		_data = string(data, length);
	}
	virtual const void setMeta(const string& key, const string& value)  {
		_meta[key] = value;
	}
	virtual const map<string, string>& getMeta()                        {
		return _meta;
	}
	virtual const string& getMeta(const string& key)                    {
		return _meta[key];
	}
	/// Simplified access to keyset for Java, namespace qualifiers required for swig!
	virtual const std::vector<std::string>& getKeys() {
		map<string, string>::const_iterator metaIter;
		_keys.clear();
		for (metaIter = _meta.begin(); metaIter != _meta.end(); metaIter++) {
			_keys.push_back(metaIter->first);
		}
		return _keys;
	}

	static Message* toSubscriber(const string& uuid) {
		Message* msg = new Message();
		msg->setMeta("subscriber", uuid);
		return msg;
	}

protected:
	string _data;
	map<string, string> _meta;
	vector<string> _keys;
};
}


#endif /* end of include guard: MESSAGE_H_Y7TB6U8 */

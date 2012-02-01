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
 * Message implementor basis class (bridge pattern).
 */
class MessageImpl {
public:
	virtual char* getData() = 0;
	virtual void setData(char*) = 0;

	virtual int getSize() = 0;
	virtual void setSize(int) = 0;

	virtual char* getRaw() = 0;
	virtual void setRaw(char*) = 0;

	virtual int getInt(std::string) = 0;
	virtual void setInt(std::string, int) = 0;

	virtual std::string getString(std::string) = 0;
	virtual void setString(std::string, std::string) = 0;
};

/**
 * Definition of message types and abstraction of message - not used (bridge pattern).
 *
 * We are still pondering whether we need an explicit message representation. At the moment, only the Type enum is used.
 */
class Message : public boost::enable_shared_from_this<Message> {
public:
	enum Type {
	    DATA          = 0x0000,
	    PUB_ADDED     = 0x0004,
	    PUB_REMOVED   = 0x0005,
	    NODE_INFO     = 0x0006,
	};

	Message();
	virtual ~Message();

	virtual char* getData();
	virtual void setData(char*);

	virtual int getSize();
	virtual void setSize(int);

	virtual char* getRaw();
	virtual void setRaw(char*);

	virtual int getInt(std::string);
	virtual void setInt(std::string, int);

	virtual std::string getString(std::string);
	virtual void setString(std::string, std::string);

protected:
	boost::shared_ptr<MessageImpl> _impl;

};
}


#endif /* end of include guard: MESSAGE_H_Y7TB6U8 */

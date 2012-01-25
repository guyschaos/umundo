#ifndef ZEROMQMESSAGE_H_5WHSYXBJ
#define ZEROMQMESSAGE_H_5WHSYXBJ

#include "common/Message.h"

namespace umundo {

class ZeroMQMessage : public MessageImpl {
public:
	ZeroMQMessage();
	virtual ~ZeroMQMessage();

	char* getData();
	void setData(char*);

	int getSize();
	void setSize(int);

	char* getRaw();
	void setRaw(char*);

	int getInt(std::string);
	void setInt(std::string, int);

	std::string getString(std::string);
	void setString(std::string, std::string);

protected:
	zmq_msg_t message;

private:
	ZeroMQMessage(const ZeroMQMessage &other) {}
	ZeroMQMessage& operator= (const ZeroMQMessage &other) {
		return *this;
	}

};

}

#endif /* end of include guard: ZEROMQMESSAGE_H_5WHSYXBJ */

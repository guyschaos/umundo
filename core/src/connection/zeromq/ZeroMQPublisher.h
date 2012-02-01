#ifndef ZEROMQPUBLISHER_H_AX5HLY5Q
#define ZEROMQPUBLISHER_H_AX5HLY5Q

#include "common/stdInc.h"
#include "connection/Publisher.h"

#include <zmq.h>

#define ZEROMQ_PUB_HWM 16

namespace umundo {

/**
 * Concrete publisher implementor for 0MQ (bridge pattern).
 */
class ZeroMQPublisher : public PublisherImpl, public boost::enable_shared_from_this<ZeroMQPublisher>  {
public:
	PublisherImpl* create(string);
	virtual ~ZeroMQPublisher();

	void send(char* buffer, size_t length);
	uint16_t getPort();
protected:
	/**
	* Constructor used for prototype in Factory only.
	*/
	ZeroMQPublisher() {};

	/**
	 * Actual instance as instantiated by create()
	 */
	ZeroMQPublisher(string);

private:
	ZeroMQPublisher(const ZeroMQPublisher &other) {}
	ZeroMQPublisher& operator= (const ZeroMQPublisher &other) {
		return *this;
	}

	void* _socket;
	void* _zeroMQCtx;
	uint16_t _port;
	string _transport;

	friend class Factory;
};

}

#endif /* end of include guard: ZEROMQPUBLISHER_H_AX5HLY5Q */

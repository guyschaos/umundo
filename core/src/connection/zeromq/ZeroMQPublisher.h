#ifndef ZEROMQPUBLISHER_H_AX5HLY5Q
#define ZEROMQPUBLISHER_H_AX5HLY5Q

#include "common/stdInc.h"
#include "common/Implementation.h"
#include "connection/Publisher.h"

#include <zmq.h>

#define ZEROMQ_PUB_HWM 16

namespace umundo {

/**
 * Concrete publisher implementor for 0MQ (bridge pattern).
 */
class ZeroMQPublisher : public PublisherImpl, public boost::enable_shared_from_this<ZeroMQPublisher>  {
public:
	virtual ~ZeroMQPublisher();
	
	shared_ptr<Implementation> create();
	void init(shared_ptr<Configuration>);
	void destroy();

	void send(const char* buffer, size_t length);
protected:
	/**
	* Constructor used for prototype in Factory only.
	*/
	ZeroMQPublisher();

private:
	// ZeroMQPublisher(const ZeroMQPublisher &other) {}
	// ZeroMQPublisher& operator= (const ZeroMQPublisher &other) {
	// 	return *this;
	// }

	void* _socket;
	void* _zeroMQCtx;
	shared_ptr<PublisherConfig> _config;

	friend class Factory;
};

}

#endif /* end of include guard: ZEROMQPUBLISHER_H_AX5HLY5Q */

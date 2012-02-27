#ifndef ZEROMQPUBLISHER_H_AX5HLY5Q
#define ZEROMQPUBLISHER_H_AX5HLY5Q

#include <boost/enable_shared_from_this.hpp>

#include "umundo/common/Common.h"
#include "umundo/connection/Publisher.h"
#include "umundo/thread/Thread.h"

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

	void send(Message* msg);
  int waitForSubscribers(int count);
  
protected:
	/**
	* Constructor used for prototype in Factory only.
	*/
	ZeroMQPublisher();
  void addedSubscriber();
  void removedSubscriber();

private:
	// ZeroMQPublisher(const ZeroMQPublisher &other) {}
	// ZeroMQPublisher& operator= (const ZeroMQPublisher &other) {
	// 	return *this;
	// }

	void* _socket;
	void* _zeroMQCtx;
	shared_ptr<PublisherConfig> _config;
  int _pubCount;
  Mutex _pubLock;

	friend class Factory;
	friend class ZeroMQNode;
	friend class Publisher;
};

}

#endif /* end of include guard: ZEROMQPUBLISHER_H_AX5HLY5Q */

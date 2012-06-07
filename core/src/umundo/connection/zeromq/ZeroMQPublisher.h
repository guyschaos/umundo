#ifndef ZEROMQPUBLISHER_H_AX5HLY5Q
#define ZEROMQPUBLISHER_H_AX5HLY5Q

#include <boost/enable_shared_from_this.hpp>

#include "umundo/common/Common.h"
#include "umundo/connection/Publisher.h"
#include "umundo/thread/Thread.h"

namespace umundo {

class ZeroMQNode;

/**
 * Concrete publisher implementor for 0MQ (bridge pattern).
 */
class ZeroMQPublisher : public PublisherImpl, public Thread, public boost::enable_shared_from_this<ZeroMQPublisher>  {
public:
	virtual ~ZeroMQPublisher();

	shared_ptr<Implementation> create(void*);
	void init(shared_ptr<Configuration>);
	void destroy();
	void suspend();
	void resume();

	void send(Message* msg);
	int waitForSubscribers(int count);

protected:
	/**
	* Constructor used for prototype in Factory only.
	*/
	ZeroMQPublisher();
	void addedSubscriber(const string, const string);
	void removedSubscriber(const string, const string);

private:
	// ZeroMQPublisher(const ZeroMQPublisher &other) {}
	// ZeroMQPublisher& operator= (const ZeroMQPublisher &other) {
	// 	return *this;
	// }

  // read subscription requests from publisher socket
	void run();
  void join();
  
	void* _socket;
	void* _closer;
	void* _zeroMQCtx;
	shared_ptr<PublisherConfig> _config;

  /**
   * To ensure solid subscriptions, we receive them twice,
   * once through the node socket and once through the
   * xpub socket, only when both have been received do we
   * we signal the greeters.
   */
  set<string> _pendingZMQSubscriptions;
  map<string, string> _pendingSubscriptions;
  map<string, string> _subscriptions;
  
	Monitor _pubLock;
	Mutex _mutex;

	friend class Factory;
	friend class ZeroMQNode;
	friend class Publisher;
};

}

#endif /* end of include guard: ZEROMQPUBLISHER_H_AX5HLY5Q */

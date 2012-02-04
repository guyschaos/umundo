#ifndef ZEROMQSUBSCRIBER_H_6DV3QJUH
#define ZEROMQSUBSCRIBER_H_6DV3QJUH

#include <boost/enable_shared_from_this.hpp>

#include "common/stdInc.h"

#include "common/ResultSet.h"
#include "connection/Subscriber.h"
#include "connection/Publisher.h"

namespace umundo {

/**
 * Concrete subscriber implementor for 0MQ (bridge pattern).
 */
class ZeroMQSubscriber : public SubscriberImpl, public ResultSet<PublisherStub>, public boost::enable_shared_from_this<ZeroMQSubscriber> {
public:
	shared_ptr<Implementation> create();
	void destroy();
	void init(shared_ptr<Configuration>);
	virtual ~ZeroMQSubscriber();

	void added(shared_ptr<PublisherStub>);
	void removed(shared_ptr<PublisherStub>);
	void changed(shared_ptr<PublisherStub>);

	// Thread
	void run();

protected:
	ZeroMQSubscriber();
	ZeroMQSubscriber(string, Receiver*);

	void* _socket;
	void* _zeroMQCtx;

private:

	shared_ptr<SubscriberConfig> _config;
	friend class Factory;
};

}

#endif /* end of include guard: ZEROMQSUBSCRIBER_H_6DV3QJUH */

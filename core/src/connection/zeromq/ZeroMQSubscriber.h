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
	SubscriberImpl* create(string, Receiver*);
	virtual ~ZeroMQSubscriber();

	const string& getChannelName();

	void added(shared_ptr<PublisherStub>);
	void removed(shared_ptr<PublisherStub>);
	void changed(shared_ptr<PublisherStub>);

	// Thread
	void run();

protected:
	ZeroMQSubscriber();
	ZeroMQSubscriber(string, Receiver*);

	string _channelName;
	Receiver* _receiver;

	void* _socket;
	void* _zeroMQCtx;

private:
	ZeroMQSubscriber(const ZeroMQSubscriber &other) {}
	ZeroMQSubscriber& operator= (const ZeroMQSubscriber &other) {
		return *this;
	}


	friend class Factory;
};

}

#endif /* end of include guard: ZEROMQSUBSCRIBER_H_6DV3QJUH */

#ifndef ZEROMQDISPATCHER_H_XFMTSVLV
#define ZEROMQDISPATCHER_H_XFMTSVLV

#include <zmq.h>
#include <boost/enable_shared_from_this.hpp>

#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"
#include "umundo/common/ResultSet.h"
#include "umundo/common/Node.h"

namespace umundo {

class PublisherStub;
class ZeroMQPublisher;
class ZeroMQSubscriber;
class NodeQuery;

/**
 * Concrete node implementor for 0MQ (bridge pattern).
 */
class ZeroMQNode : public Thread, public ResultSet<NodeStub>, public NodeImpl, public boost::enable_shared_from_this<ZeroMQNode> {
public:
	virtual ~ZeroMQNode();

	/** @name Implementor */
	//@{
	shared_ptr<Implementation> create();
	void destroy();
	void init(shared_ptr<Configuration>);
	//@}

	/** @name Publish / Subscriber Maintenance */
	//@{
	void addSubscriber(shared_ptr<SubscriberImpl>);
	void removeSubscriber(shared_ptr<SubscriberImpl>);
	void addPublisher(shared_ptr<PublisherImpl>);
	void removePublisher(shared_ptr<PublisherImpl>);
	//@}

	/** @name Callbacks from Discovery */
	//@{
	void added(shared_ptr<NodeStub>);    ///< A node was added, connect to its router socket and list our publishers.
	void removed(shared_ptr<NodeStub>);  ///< A node was removed, notify local subscribers and clean up.
	void changed(shared_ptr<NodeStub>);  ///< Never happens.
	//@}

	static void* getZeroMQContext();

protected:
	ZeroMQNode();

	void run(); ///< see Thread

	/** @name Control message handling */
	//@{
	zmq_msg_t msgPubList();
	void processPubAdded(const char*, zmq_msg_t);
	void processPubRemoved(const char*, zmq_msg_t);
	//@}


	/** @name Remote subscriber maintenance */
	//@{
	void processSubscription(const char*, zmq_msg_t);
	void processUnsubscription(const char*, zmq_msg_t);
	void notifyOfUnsubscription(void*, shared_ptr<ZeroMQSubscriber>, shared_ptr<PublisherStub>);
	void notifyOfSubscription(void*, shared_ptr<ZeroMQSubscriber>, shared_ptr<PublisherStub>);

	//@}

	/** @name Local subscriber maintenance */
	//@{
	void addRemotePubToLocalSubs(const char*, shared_ptr<PublisherStub>); ///< See if we have a local Subscriber interested in the remote Publisher.
	void removeRemotePubFromLocalSubs(const char*, shared_ptr<PublisherStub>); ///< A remote Publisher was removed, notify Subscribe%s.
	//@}

private:
	ZeroMQNode(const ZeroMQNode &other) {}
	ZeroMQNode& operator= (const ZeroMQNode &other) {
		return *this;
	}

	void processPubSub(const char*, zmq_msg_t, bool); ///< notify local publishers about subscriptions
	bool validateState(); ///< check the nodes state

//	static void initPubMgmtMsg(zmq_msg_t&, shared_ptr<PublisherStub>); ///< prepare a control message regarding a Publisher.
	static char* writePubInfo(char*, uint16_t, const char*); ///< write publisher info into given byte array
	static char* readPubInfo(char*, uint16_t&, char*&); ///< read publisher from into given byte array

	static void* _zmqContext; ///< global 0MQ context.
	void* _responder; ///< 0MQ node socket for administrative messages.
	Mutex _mutex;
	shared_ptr<NodeQuery> _nodeQuery; ///< the NodeQuery which we registered at the Discovery sub-system.

	map<string, shared_ptr<NodeStub> > _nodes;                                    ///< UUIDs to NodeStub%s.
	map<string, void*> _sockets;                                                  ///< UUIDs to ZeroMQ Sockets.
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _remotePubs;          ///< UUIDs to ports to remote publishers.
	map<string, map<uint16_t, int > > _remoteSubs;                                ///< UUIDs to ports to number of remote subscribers.
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _pendingPubAdditions; ///< received publishers of yet undiscovered nodes.
	map<uint16_t, shared_ptr<ZeroMQPublisher> > _localPubs;                       ///< Local ports to local publishers.
	set<shared_ptr<ZeroMQSubscriber> > _localSubs;                                ///< Local subscribers.
	shared_ptr<NodeConfig> _config;

	static shared_ptr<ZeroMQNode> _instance; ///< Singleton instance.

	friend std::ostream& operator<<(std::ostream&, const ZeroMQNode*);
	friend class Factory;
};

}

#endif /* end of include guard: ZEROMQDISPATCHER_H_XFMTSVLV */

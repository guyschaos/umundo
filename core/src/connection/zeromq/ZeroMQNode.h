#ifndef ZEROMQDISPATCHER_H_XFMTSVLV
#define ZEROMQDISPATCHER_H_XFMTSVLV

#include "common/stdInc.h"

#include <boost/lexical_cast.hpp>
#include <zmq.h>

#include "connection/zeromq/ZeroMQSubscriber.h"
#include "connection/zeromq/ZeroMQPublisher.h"
#include "common/Node.h"
#include "common/Message.h"
#include "discovery/NodeQuery.h"
#include "discovery/Discovery.h"
#include "common/ResultSet.h"

namespace umundo {

/**
 * Concrete node implementor for 0MQ (bridge pattern).
 */
class ZeroMQNode : public Thread, public ResultSet<NodeStub>, public Node {
public:
	static shared_ptr<ZeroMQNode> getInstance();
	virtual ~ZeroMQNode();

	/** @name Publish / Subscriber Maintenance */
  //@{	
	static void addSubscriber(SubscriberImpl*);
	static void removeSubscriber(SubscriberImpl*);
	static void addPublisher(PublisherImpl*);
	static void removePublisher(PublisherImpl*);
  //@}
	
	/** @name Callbacks from Discovery */
  //@{
	void added(shared_ptr<NodeStub>);    ///< A node was added, connect to its router socket and list our publishers.
	void removed(shared_ptr<NodeStub>);  ///< A node was removed, notify local subscribers and clean up.
	void changed(shared_ptr<NodeStub>);  ///< Never happens.
  //@}

protected:
	ZeroMQNode();

	void run(); ///< see Thread

	/** @name Control message handling */
  //@{
	zmq_msg_t msgPubList();
	void processPubAdded(const char*, zmq_msg_t);
	void processPubRemoved(const char*, zmq_msg_t);
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

	static void initPubMgmtMsg(zmq_msg_t&, ZeroMQPublisher*); ///< prepare a control message regarding a Publisher.

	void* _zeroMQCtx; ///< 0MQ context - yes, we have a private one - not sure if we want one shared among all publishers as well.
	void* _responder; ///< 0MQ node socket for administrative messages.
	Mutex _mutex;
	NodeQuery* _nodeQuery; ///< the NodeQuery which we registered at the Discovery sub-system.

	map<string, shared_ptr<NodeStub> > _nodes;                                    ///< UUIDs to NodeStub%s.
	map<string, void*> _sockets;                                                  ///< UUIDs to ZeroMQ Sockets.
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _remotePubs;          ///< UUIDs to ports to remote publishers.
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _pendingPubAdditions; ///< received publishers of yet undiscovered nodes.
	map<uint16_t, ZeroMQPublisher* > _localPubs;                       ///< Local ports to local publishers.
	set<ZeroMQSubscriber* > _localSubs;                                ///< Local subscribers.

	static shared_ptr<ZeroMQNode> _instance; ///< Singleton instance.

	friend std::ostream& operator<<(std::ostream&, const ZeroMQNode*);

};

}

#endif /* end of include guard: ZEROMQDISPATCHER_H_XFMTSVLV */

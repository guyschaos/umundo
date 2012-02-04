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
class ZeroMQNode : public Thread, public ResultSet<NodeStub>, public NodeImpl {
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

	static void initPubMgmtMsg(zmq_msg_t&, shared_ptr<ZeroMQPublisher>); ///< prepare a control message regarding a Publisher.

	static void* _zmqContext; ///< global 0MQ context.
	void* _responder; ///< 0MQ node socket for administrative messages.
	Mutex _mutex;
	shared_ptr<NodeQuery> _nodeQuery; ///< the NodeQuery which we registered at the Discovery sub-system.

	map<string, shared_ptr<NodeStub> > _nodes;                                    ///< UUIDs to NodeStub%s.
	map<string, void*> _sockets;                                                  ///< UUIDs to ZeroMQ Sockets.
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _remotePubs;          ///< UUIDs to ports to remote publishers.
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

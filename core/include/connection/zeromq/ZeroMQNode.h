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

class ZeroMQNode : public Thread, public ResultSet<NodeStub>, public Node {
public:
	static shared_ptr<ZeroMQNode> getInstance();
	virtual ~ZeroMQNode();

	// pub sub maintenance
	static void addSubscriber(SubscriberImpl*);
	static void removeSubscriber(SubscriberImpl*);
	static void addPublisher(PublisherImpl*);
	static void removePublisher(PublisherImpl*);

	// ResultSet<NodeStub> from discovery
	void added(shared_ptr<NodeStub>);
	void removed(shared_ptr<NodeStub>);
	void changed(shared_ptr<NodeStub>);

protected:
	ZeroMQNode();

	// Thread
	void run();

	// control message handling
	zmq_msg_t msgPubList();
	void processPubAdded(const char*, zmq_msg_t);
	void processPubRemoved(const char*, zmq_msg_t);

	// local subscriber maintenance
	void addRemotePubToLocalSubs(const char*, shared_ptr<PublisherStub>);
	void removeRemotePubFromLocalSubs(const char*, shared_ptr<PublisherStub>);

private:
	ZeroMQNode(const ZeroMQNode &other) {}
	ZeroMQNode& operator= (const ZeroMQNode &other) {
		return *this;
	}

	static void initPubMgmtMsg(zmq_msg_t&, ZeroMQPublisher*);

	void* _zeroMQCtx;
	void* _responder;
	Mutex _mutex;
	NodeQuery* _nodeQuery;

	map<string, shared_ptr<NodeStub> > _nodes;                                    // uuids to NodeStubs
	map<string, void*> _sockets;                                                  // uuids to ZeroMQ Sockets
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _remotePubs;          // uuids to ports to remote publishers
	map<string, map<uint16_t, shared_ptr<PublisherStub> > > _pendingPubAdditions; // remote publishers of undiscovered nodes
	map<uint16_t, ZeroMQPublisher* > _localPubs;                       // local ports to local publishers
	set<ZeroMQSubscriber* > _localSubs;                                // local subscribers

	static shared_ptr<ZeroMQNode> _instance;

	friend std::ostream& operator<<(std::ostream&, const ZeroMQNode*);

};

}

#endif /* end of include guard: ZEROMQDISPATCHER_H_XFMTSVLV */

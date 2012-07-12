/**
 *  Copyright (C) 2012  Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 */

#ifndef ZEROMQDISPATCHER_H_XFMTSVLV
#define ZEROMQDISPATCHER_H_XFMTSVLV

#include <zmq.h>
#include <boost/enable_shared_from_this.hpp>

#include "umundo/common/Common.h"
#include "umundo/thread/Thread.h"
#include "umundo/common/ResultSet.h"
#include "umundo/connection/Node.h"

/// Initialize a zeromq message with a given size
#define ZMQ_PREPARE(msg, size) \
zmq_msg_init(&msg) && LOG_WARN("zmq_msg_init: %s", zmq_strerror(errno)); \
zmq_msg_init_size (&msg, size) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));

/// Initialize a zeromq message with a given size and memcpy data
#define ZMQ_PREPARE_DATA(msg, data, size) \
zmq_msg_init(&msg) && LOG_WARN("zmq_msg_init: %s", zmq_strerror(errno)); \
zmq_msg_init_size (&msg, size) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno)); \
memcpy(zmq_msg_data(&msg), data, size);

/// Initialize a zeromq message with a given null-terminated string
#define ZMQ_PREPARE_STRING(msg, data, size) \
zmq_msg_init(&msg) && LOG_WARN("zmq_msg_init: %s", zmq_strerror(errno)); \
zmq_msg_init_size (&msg, size + 1) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno)); \
memcpy(zmq_msg_data(&msg), data, size + 1);

/// Size of a publisher info on wire
#define PUB_INFO_SIZE(pub) \
pub->getChannelName().length() + 1 + pub->getUUID().length() + 1 + 2

namespace umundo {

class PublisherStub;
class ZeroMQPublisher;
class ZeroMQSubscriber;
class NodeQuery;

/**
 * Concrete node implementor for 0MQ (bridge pattern).
 */
class DLLEXPORT ZeroMQNode : public Thread, public ResultSet<NodeStub>, public NodeImpl, public boost::enable_shared_from_this<ZeroMQNode> {
public:
	virtual ~ZeroMQNode();

	/** @name Implementor */
	//@{
	shared_ptr<Implementation> create(void*);
	void destroy();
	void init(shared_ptr<Configuration>);
	void suspend();
	void resume();
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

	/** @name uMundo deployment object model */
	//@{
//	set<NodeStub*> getAllNodes();
	//@}


	static void* getZeroMQContext();

protected:
	ZeroMQNode();

	void run(); ///< see Thread
	void join(); ///< Overridden to unblock zmq_recv_msg in run()

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

	void processSubscription(const char*, zmq_msg_t, bool); ///< notify local publishers about subscriptions
	bool validateState(); ///< check the nodes state

	static char* writePubInfo(char*, shared_ptr<PublisherStub>); ///< write publisher info into given byte array
	static char* readPubInfo(char*, uint16_t&, char*&, char*&); ///< read publisher from given byte array
	static char* writeSubInfo(char*, shared_ptr<ZeroMQSubscriber>); ///< write subscriber info into given byte array
	static char* readSubInfo(char*, char*&); ///< read subscriber from given byte array

	static void* _zmqContext; ///< global 0MQ context.
	void* _responder; ///< 0MQ node socket for administrative messages.
	shared_ptr<NodeQuery> _nodeQuery; ///< the NodeQuery which we registered at the Discovery sub-system.
	Mutex _mutex;

	map<string, shared_ptr<NodeStub> > _nodes;                                    ///< UUIDs to remote NodeStub%s.
	map<string, void*> _sockets;                                                  ///< UUIDs to ZeroMQ Node Sockets.

	map<string, map<string, shared_ptr<PublisherStub> > > _remotePubs;            ///< remote node UUIDs to remote publisher UUIDs.
	map<string, map<string, shared_ptr<PublisherStub> > > _pendingRemotePubs;     ///< publishers of yet undiscovered nodes.
	map<string, map<string, set<string> > > _subscriptions;                       ///< remote node UUIDs to local publisher UUID to remote subscriber UUIDs.

	map<string, shared_ptr<ZeroMQPublisher> > _localPubs;                         ///< UUIDS to local publishers.
	set<shared_ptr<ZeroMQSubscriber> > _localSubs;                                ///< Local subscribers.
	map<string, shared_ptr<ZeroMQPublisher> > _suspendedLocalPubs;                ///< suspended publishers to be resumed.

	shared_ptr<NodeConfig> _config;

	static shared_ptr<ZeroMQNode> _instance; ///< Singleton instance.

	friend class Factory;
};

}

#endif /* end of include guard: ZEROMQDISPATCHER_H_XFMTSVLV */

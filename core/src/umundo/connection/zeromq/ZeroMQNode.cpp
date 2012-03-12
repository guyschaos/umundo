#include "umundo/connection/zeromq/ZeroMQNode.h"

#include "umundo/config.h"

#if defined UNIX || defined IOS || defined IOSSIM
#include <arpa/inet.h> // htons
#include <string.h> // strlen, memcpy
#include <stdio.h> // snprintf
#endif

#include <boost/lexical_cast.hpp>

#include "umundo/common/Message.h"
#include "umundo/discovery/Discovery.h"
#include "umundo/discovery/NodeQuery.h"
#include "umundo/connection/zeromq/ZeroMQPublisher.h"
#include "umundo/connection/zeromq/ZeroMQSubscriber.h"

namespace umundo {

ZeroMQNode::~ZeroMQNode() {
	DEBUG_DTOR("ZeroMQNode");
  Discovery::unbrowse(_nodeQuery);
	stop();
  
  // this is a hack .. the thread is blocking at zmq_recvmsg - unblock by sending a message
  void* closer;
  (closer = zmq_socket(getZeroMQContext(), ZMQ_DEALER)) || LOG_ERR("zmq_socket: %s",zmq_strerror(errno));  
  std::stringstream ss;
	ss << _transport << "://127.0.0.1:" << _port;
  zmq_connect(closer, ss.str().c_str()) && LOG_WARN("zmq_connect: %s",zmq_strerror(errno));
  zmq_msg_t msg;
	ZMQ_PREPARE_STRING(msg, "quit", 4);
  
	zmq_sendmsg(closer, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
  while(zmq_close(closer) != 0) {
    LOG_WARN("zmq_close: %s - retrying", zmq_strerror(errno));
    Thread::sleepMs(50);
  }
  
  map<string, void*>::iterator sockIter;
  for (sockIter = _sockets.begin(); sockIter != _sockets.end(); sockIter++) {
    while(zmq_close(sockIter->second) != 0) {
      LOG_WARN("zmq_close: %s - retrying", zmq_strerror(errno));
      Thread::sleepMs(50);
    }
  }
  
	join();
	if (_responder)
    while(zmq_close(_responder) != 0) {
      LOG_WARN("zmq_close: %s - retrying", zmq_strerror(errno));
      Thread::sleepMs(50);
    }
}

ZeroMQNode::ZeroMQNode() {
	DEBUG_CTOR("ZeroMQNode");
}

shared_ptr<Implementation> ZeroMQNode::create() {
	return shared_ptr<ZeroMQNode>(new ZeroMQNode());
}

void ZeroMQNode::destroy() {
	delete(this);
}

void* ZeroMQNode::getZeroMQContext() {
	if (_zmqContext == NULL) {
		(_zmqContext = zmq_init(1)) || LOG_ERR("zmq_init: %s",zmq_strerror(errno));
	}
	return _zmqContext;
}
void* ZeroMQNode::_zmqContext = NULL;

void ZeroMQNode::init(shared_ptr<Configuration> config) {
	_config = boost::static_pointer_cast<NodeConfig>(config);
	_transport = "tcp";

	_nodeQuery = shared_ptr<NodeQuery>(new NodeQuery(_domain, this));
	(_responder = zmq_socket(getZeroMQContext(), ZMQ_ROUTER))  || LOG_ERR("zmq_socket: %s",zmq_strerror(errno));

	// start with 4242 and work your way up until we find a free port
	int port = 4242;

	std::stringstream ss;
	ss << _transport << "://*:" << port;

	LOG_DEBUG("trying to bind at %s", ss.str().c_str());

	while(zmq_bind(_responder, ss.str().c_str()) == -1) {
		switch(errno) {
		case EADDRINUSE:
			port++;
			ss.clear();        // clear error bits
			ss.str(string());  // reset string
			ss << _transport << "://*:" << port;
			break;
		default:
			LOG_WARN("zmq_bind: %s",zmq_strerror(errno));
			Thread::sleepMs(100);
		}
	}
	_port = port;
	LOG_INFO("Node %s listening as %s", SHORT_UUID(_uuid).c_str(), ss.str().c_str());

	start();
	Discovery::browse(_nodeQuery);

}

/**
 * Read from node socket and dispatch message type.
 */
void ZeroMQNode::run() {
	char* remoteId = NULL;
	int32_t more;
	size_t more_size = sizeof(more);

	while(isStarted()) {
		// read a whole envelope
		while (1) {
			zmq_msg_t message;
			zmq_msg_init(&message) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
      zmq_recvmsg(_responder, &message, 0) || LOG_WARN("zmq_recvmsg: %s",zmq_strerror(errno));

      // we will stop the thread by sending an empty packet to unblock in our destructor
      if (!isStarted()) {
        zmq_msg_close(&message) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
        return;
      }
      
			int msgSize = zmq_msg_size(&message);

			if (msgSize == 36 && remoteId == NULL) {
				// remote id from envelope
				remoteId = (char*)malloc(37);
				memcpy(remoteId, zmq_msg_data(&message), 36);
				remoteId[36] = 0;

			} else if (msgSize >= 2) {
				// every envelope starts with the remote uuid we read above
				assert(remoteId != NULL);
				assert(strlen(remoteId) == 36);

				// first two bytes are type of message
				uint16_t type = ntohs(*(short*)(zmq_msg_data(&message)));
				LOG_DEBUG("Received message type %s with %d bytes from %s", Message::typeToString(type), msgSize, strndup(remoteId, 8));

        _mutex.lock();

        /**
         * We may receive data and other messages from nodes that are about to vanish
         * only accept PUB_ADDED messages for unknown nodes.
         */
        if (type != Message::PUB_ADDED && _nodes.find(remoteId) == _nodes.end()) {
          zmq_msg_close(&message) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
          string remoteIdStr(remoteId);
          LOG_WARN("Ignoring message from unconnected node %s", SHORT_UUID(remoteIdStr).c_str());
          _mutex.unlock();
          continue;
        }
        
				// dispatch message type
				switch (type) {
				case Message::DATA:
					break;
				case Message::PUB_ADDED:
					processPubAdded(remoteId, message);
					break;
				case Message::PUB_REMOVED:
					processPubRemoved(remoteId, message);
					break;
				case Message::SUBSCRIBE:
					processSubscription(remoteId, message);
					break;
				case Message::UNSUBSCRIBE:
					processUnsubscription(remoteId, message);
					break;
				default:
					LOG_WARN("Received unknown message type");
				}
			} else {
				LOG_WARN("Received message without remote id in header");
			}

      _mutex.unlock();

			// are there more messages in the envelope?
			zmq_getsockopt(_responder, ZMQ_RCVMORE, &more, &more_size) && LOG_WARN("zmq_getsockopt: %s",zmq_strerror(errno));
			zmq_msg_close(&message) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));

			if (!more) {
				free(remoteId);
				remoteId = NULL;
				break; // last message part
			}
		}
	}
}

/**
 * A remote subscriber subscribed to one of our publishers
 */
void ZeroMQNode::processSubscription(const char* remoteId, zmq_msg_t message) {
	processPubSub(remoteId, message, true);
}

/**
 * A remote subscriber unsubscribed from one of our publishers
 */
void ZeroMQNode::processUnsubscription(const char* remoteId, zmq_msg_t message) {
	processPubSub(remoteId, message, false);
}

/**
 * A remote subscriber either subscribed or unsubscribed from one of our publishers.
 */
void ZeroMQNode::processPubSub(const char* remoteId, zmq_msg_t message, bool subscribe) {
	_mutex.lock();

	int msgSize = zmq_msg_size(&message);
	char* buffer = (char*)zmq_msg_data(&message);
	uint16_t port = 0;
	char* channel;
	buffer = readPubInfo(buffer + 2, port, channel);

	if (buffer - (char*)zmq_msg_data(&message) != msgSize) {
		if (subscribe) {
			LOG_ERR("Malformed SUBSCRIBE message received - ignoring");
		} else {
			LOG_ERR("Malformed UNSUBSCRIBE message received - ignoring");
		}
		assert(validateState());
		_mutex.unlock();
		return;
	}
	shared_ptr<ZeroMQPublisher> pub = _localPubs[port];
	assert(pub.get() != NULL);
	if (subscribe) {
		// remember remote node subscription to local pub for vanishing nodes
		if (_remoteSubs.find(remoteId) == _remoteSubs.end()) {
			_remoteSubs[remoteId] = map<uint16_t, int>();
    }
		if (_remoteSubs[remoteId].find(port) == _remoteSubs[remoteId].end())
			_remoteSubs[remoteId][port] = 0;
		_remoteSubs[remoteId][port] = _remoteSubs[remoteId][port] + 1;
		pub->addedSubscriber();
	} else {
		// remove subscription on remote node for publisher
		pub->removedSubscriber();
		if (_remoteSubs.find(remoteId) == _remoteSubs.end()) {
			LOG_DEBUG("Received UNSUBSCRIBE from unknown node");
			goto unknown_subscription;
		}
		if (_remoteSubs[remoteId].find(port) == _remoteSubs[remoteId].end()) {
			LOG_DEBUG("Received UNSUBSCRIBE for unsubscribed publisher");
			goto unknown_subscription;
		}
		if (_remoteSubs[remoteId][port] < 1) {
			LOG_DEBUG("Received UNSUBSCRIBE for publisher with no subscriptions");
			goto unknown_subscription;
		}
		_remoteSubs[remoteId][port] = _remoteSubs[remoteId][port] - 1;
		if (_remoteSubs[remoteId][port] == 0) {
			_remoteSubs[remoteId].erase(port);
		}
		if (_remoteSubs[remoteId].size() == 0) {
			_remoteSubs.erase(remoteId);
		}
	}
unknown_subscription:
	assert(validateState());
	_mutex.unlock();

}

/**
 * We received a publisher from a remote node
 */
void ZeroMQNode::processPubAdded(const char* remoteId, zmq_msg_t message) {
	_mutex.lock();

	int msgSize = zmq_msg_size(&message);
	char* buffer = (char*)zmq_msg_data(&message);
	uint16_t port = 0;
	char* channel;
	buffer = readPubInfo(buffer + 2, port, channel);
	if (buffer - (char*)zmq_msg_data(&message) != msgSize) {
		LOG_ERR("Malformed PUB_ADDED received - ignoring");
		assert(validateState());
		_mutex.unlock();
		return;
	}
	LOG_DEBUG("received publisher %s from %s", channel, strndup(remoteId, 8));

	shared_ptr<PublisherStub> pubStub = shared_ptr<PublisherStub>(new PublisherStub());
	pubStub->setChannelName(channel);
	pubStub->setPort(port);
	addRemotePubToLocalSubs(remoteId, pubStub);
	assert(validateState());
	_mutex.unlock();

}

void ZeroMQNode::processPubRemoved(const char* remoteId, zmq_msg_t message) {
	_mutex.lock();

	int msgSize = zmq_msg_size(&message);
	char* buffer = (char*)zmq_msg_data(&message);
	uint16_t port = 0;
	char* channel;
	buffer = readPubInfo(buffer + 2, port, channel);

	if (buffer - (char*)zmq_msg_data(&message) != msgSize) {
		LOG_ERR("Malformed PUB_REMOVED received - ignoring");
		assert(validateState());
		_mutex.unlock();
		return;
	}
	shared_ptr<PublisherStub> pubStub = _remotePubs[remoteId][port];

	if (pubStub)
		removeRemotePubFromLocalSubs(remoteId, pubStub);
	assert(validateState());
	_mutex.unlock();
}

/**
 * Notify a remote node that we will unsubscribe from a publisher.
 */
void ZeroMQNode::notifyOfUnsubscription(void* socket, shared_ptr<ZeroMQSubscriber> zSub, shared_ptr<PublisherStub> zPub) {
	zmq_msg_t msg;
	ZMQ_PREPARE(msg, zPub->getChannelName().length() + 5);

	char* buffer = (char*)zmq_msg_data(&msg);
	*(uint16_t*)(buffer) = htons(Message::UNSUBSCRIBE);
	writePubInfo(buffer + 2, zPub->getPort(), zPub->getChannelName().c_str());

	zmq_sendmsg(socket, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
}

/**
 * Notify a remote node that we will subscribe to a publisher.
 */
void ZeroMQNode::notifyOfSubscription(void* socket, shared_ptr<ZeroMQSubscriber> zSub, shared_ptr<PublisherStub> zPub) {
	zmq_msg_t msg;
	ZMQ_PREPARE(msg, zPub->getChannelName().length() + 5);

	char* buffer = (char*)zmq_msg_data(&msg);
	*(uint16_t*)(buffer) = htons(Message::SUBSCRIBE);
	writePubInfo(buffer + 2, zPub->getPort(), zPub->getChannelName().c_str());

	zmq_sendmsg(socket, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
}

/**
\msc
  "Remote ZeroMQNode",Discovery,ZeroMQNode;
  Discovery->"Remote ZeroMQNode" [label="discovered"];
	Discovery->ZeroMQNode [label="added(NodeStub)", URL="\ref NodeStub"];
	ZeroMQNode->"Remote ZeroMQNode" [label="establish connection on node socket"];
	ZeroMQNode->"Remote ZeroMQNode" [label="List of all local publishers", URL="\ref Publisher" ];
\endmsc
 *
 * This will cause processPubAdded() to be called for every Publisher we list.<br />
 * Keep in mind that the remote node will do the same!
 */
void ZeroMQNode::added(shared_ptr<NodeStub> node) {
	assert(node);
	assert(node->getUUID().length() == 36);
	if (node->getUUID().compare(_uuid) != 0) {
		LOG_INFO("%s added %s at %s", SHORT_UUID(_uuid).c_str(), SHORT_UUID(node->getUUID()).c_str(), node->getIP().c_str());

		// we found ourselves a remote node, lets get some privacy
		_mutex.lock();

		std::stringstream ss;
		ss << node->getTransport() << "://" << node->getIP() << ":" << node->getPort();
		LOG_DEBUG("ZeroMQNode::added connecting to %s", ss.str().c_str());

		// connect a socket to the remote node port
		void* client;
		(client = zmq_socket(getZeroMQContext(), ZMQ_DEALER)) || LOG_ERR("zmq_socket: %s",zmq_strerror(errno));

		// give the socket an id for zeroMQ routing
		zmq_setsockopt(client, ZMQ_IDENTITY, _uuid.c_str(), _uuid.length()) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));
		zmq_connect(client, ss.str().c_str()) && LOG_WARN("zmq_connect: %s",zmq_strerror(errno));

		// remember node stub and socket
		_nodes[node->getUUID()] = node;
		_sockets[node->getUUID()] = client;

		// send all our local publisher channelnames as "short PUB_ADDED:string CHANNELNAME:short PORT:\0"
		map<uint16_t, shared_ptr<ZeroMQPublisher> >::iterator pubIter;
		int hasMore = _localPubs.size() - 1;
		for (pubIter = _localPubs.begin(); pubIter != _localPubs.end(); pubIter++, hasMore--) {

			shared_ptr<ZeroMQPublisher> zPub = pubIter->second;
			assert(zPub);

			// create a publisher added message from current publisher
			zmq_msg_t msg;
			ZMQ_PREPARE(msg, zPub->getChannelName().length() + 5);

			char* buffer = (char*)zmq_msg_data(&msg);
			*(uint16_t*)(buffer) = htons(Message::PUB_ADDED);
			char* end = writePubInfo(buffer + 2, zPub->getPort(), zPub->getChannelName().c_str());
			(void)end;
			assert(end - buffer == (int)zPub->getChannelName().length() + 5);

			if (hasMore) {
				zmq_sendmsg(client, &msg, ZMQ_SNDMORE) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			} else {
				zmq_sendmsg(client, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
				LOG_DEBUG("sending %d publishers to newcomer", _localPubs.size());
			}
			zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
		}
		addRemotePubToLocalSubs(node->getUUID().c_str(), shared_ptr<PublisherStub>());
		assert(validateState());
		_mutex.unlock();
	}
}

void ZeroMQNode::removed(shared_ptr<NodeStub> node) {
	if (node->getUUID().compare(_uuid) != 0) {
		std::stringstream nodeDesc;
		nodeDesc << node;
		LOG_INFO("%s removed %s", SHORT_UUID(_uuid).c_str(), SHORT_UUID(node->getUUID()).c_str());

		_mutex.lock();

		// unregister local subscribers from remote nodes publishers
		if (_nodes.find(node->getUUID()) != _nodes.end()) {
			map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
			// iterate all remote publishers
			for (pubIter = _remotePubs[node->getUUID()].begin(); pubIter != _remotePubs[node->getUUID()].end(); pubIter++) {
				removeRemotePubFromLocalSubs(node->getUUID().c_str(), pubIter->second);
			}
		}

    if (_remoteSubs.find(node->getUUID()) != _remoteSubs.end()) {
      map<uint16_t, int >::iterator subIter;
      for (subIter = _remoteSubs[node->getUUID()].begin(); subIter != _remoteSubs[node->getUUID()].end(); subIter++) {
        if (_localPubs.find(subIter->first) != _localPubs.end()) {
          _localPubs[subIter->first]->removedSubscriber();
        } else {
          LOG_DEBUG("Removed node was subscribed to non-existent publisher");
        }
      }
    }
    
		if (_sockets.find(node->getUUID()) == _sockets.end()) {
			LOG_WARN("Removed client that was never added: %s", SHORT_UUID(node->getUUID()).c_str());
			assert(validateState());
			_mutex.unlock();
			return;
		}

		zmq_close(_sockets[node->getUUID()]) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));

		_sockets.erase(node->getUUID());               // delete socket
		_remotePubs.erase(node->getUUID());            // remove all references to remote nodes pubs
		_remoteSubs.erase(node->getUUID());            // remove all references to remote nodes subs
		_nodes.erase(node->getUUID());                 // remove node itself
		_pendingPubAdditions.erase(node->getUUID());   // I don't know whether this is needed, but it cant be wrong

//		assert(_sockets.size() == _remotePubs.size());
		assert(_sockets.size() == _nodes.size());
		assert(validateState());
		_mutex.unlock();

	}
}

void ZeroMQNode::changed(shared_ptr<NodeStub> node) {
	if (node->getUUID().compare(_uuid) != 0) {
//		LOG_INFO("%s changed %s", SHORT_UUID(_uuid).c_str(), SHORT_UUID(node->getUUID()).c_str());
	}
}

void ZeroMQNode::addRemotePubToLocalSubs(const char* remoteId, shared_ptr<PublisherStub> pub) {
	_mutex.lock();

	// ZeroMQNode::added will call us without a publisher
	if (pub)
		_pendingPubAdditions[remoteId][pub->getPort()] = pub;

	if (_nodes.find(remoteId) != _nodes.end() && _pendingPubAdditions.find(remoteId) != _pendingPubAdditions.end()) {
		void* nodeSocket = _sockets[remoteId];
		assert(nodeSocket != NULL);
		// we have discovered the node already and are ready to go
		set<shared_ptr<ZeroMQSubscriber> >::iterator subIter;
		map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
		for (pubIter = _pendingPubAdditions[remoteId].begin(); pubIter != _pendingPubAdditions[remoteId].end(); pubIter++) {
			// copy pubStub from pendingPubs to remotePubs
			_remotePubs[remoteId][pubIter->second->getPort()] = pubIter->second;

			// set publisherstub endpoint data from node
			// publishers are not bound to a node as they can be added to multiple nodes
			pubIter->second->setHost(_nodes[remoteId]->getHost());
			pubIter->second->setDomain(_nodes[remoteId]->getDomain());
			pubIter->second->setTransport(_nodes[remoteId]->getTransport());
			pubIter->second->setIP(_nodes[remoteId]->getIP());

			// see if we have a local subscriber interested in the publisher's channel
			for (subIter = _localSubs.begin(); subIter != _localSubs.end(); subIter++) {

				shared_ptr<ZeroMQSubscriber> zSub = *subIter;
				assert(zSub.get());

				if (zSub->getChannelName().compare(pubIter->second->getChannelName()) == 0) {
					zSub->added(pubIter->second);
					notifyOfSubscription(nodeSocket, zSub, pubIter->second);
				}
			}
		}
		_pendingPubAdditions[remoteId].clear();
	}
	_mutex.unlock();
}

void ZeroMQNode::removeRemotePubFromLocalSubs(const char* remoteId, shared_ptr<PublisherStub> pub) {
	_mutex.lock();

	set<shared_ptr<ZeroMQSubscriber> >::iterator subIter;
	for (subIter = _localSubs.begin(); subIter != _localSubs.end(); subIter++) {

		shared_ptr<ZeroMQSubscriber> zSub = *subIter;
		assert(zSub.get());

		if (zSub->getChannelName().compare(pub->getChannelName()) == 0) {
			zSub->removed(pub);
		}
	}
	_mutex.unlock();
}

/**
 * Add a local subscriber.
 *
 * This will call added(pub) for every known publisher with a matching channelname.
 */
void ZeroMQNode::addSubscriber(shared_ptr<SubscriberImpl> sub) {
	shared_ptr<ZeroMQSubscriber> zSub = boost::static_pointer_cast<ZeroMQSubscriber>(sub);
	if (zSub.get() == NULL)
		return;

	_mutex.lock();

	_localSubs.insert(zSub);
	map<string, map<uint16_t, shared_ptr<PublisherStub> > >::iterator nodeIdIter;
	map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
	for (nodeIdIter = _remotePubs.begin(); nodeIdIter != _remotePubs.end(); nodeIdIter++) {
		void* nodeSocket = _sockets[nodeIdIter->first];
		assert(nodeSocket != NULL);
		for (pubIter = _remotePubs[nodeIdIter->first].begin(); pubIter != _remotePubs[nodeIdIter->first].end(); pubIter++) {
			if (zSub->getChannelName().compare(pubIter->second->getChannelName()) == 0) {
				zSub->added(pubIter->second);
				notifyOfSubscription(nodeSocket, zSub, pubIter->second);
			}
		}
	}
	assert(validateState());
	_mutex.unlock();
}

/**
 * Remove a local subscriber.
 *
 * This will call removed(pub) for every publisher with a matching channelname.
 */
void ZeroMQNode::removeSubscriber(shared_ptr<SubscriberImpl> sub) {
	shared_ptr<ZeroMQSubscriber> zSub = boost::static_pointer_cast<ZeroMQSubscriber>(sub);
	if (zSub.get() == NULL)
		return;

	// do we know this subscriber?
	if (_localSubs.find(zSub) == _localSubs.end())
		return;

	_mutex.lock();
	// disconnect all publishers
	map<string, map<uint16_t, shared_ptr<PublisherStub> > >::iterator nodeIdIter;
	map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
	for (nodeIdIter = _remotePubs.begin(); nodeIdIter != _remotePubs.end(); nodeIdIter++) {
		void* nodeSocket = _sockets[nodeIdIter->first];
		assert(nodeSocket != NULL);
		for (pubIter = _remotePubs[nodeIdIter->first].begin(); pubIter != _remotePubs[nodeIdIter->first].end(); pubIter++) {
			if (zSub->getChannelName().compare(pubIter->second->getChannelName()) == 0) {
				notifyOfUnsubscription(nodeSocket, zSub, pubIter->second);
				zSub->removed(pubIter->second);
			}
		}
	}
	assert(validateState());
	_mutex.unlock();

}

void ZeroMQNode::addPublisher(shared_ptr<PublisherImpl> pub) {
	shared_ptr<ZeroMQPublisher> zPub = boost::static_pointer_cast<ZeroMQPublisher>(pub);
	if (zPub.get() == NULL) {
		LOG_WARN("Given publisher cannot be cast to ZeroMQPublisher or is NULL");
		return;
	}

	_mutex.lock();
	// do we already now this publisher?
	if (_localPubs.find(zPub->getPort()) == _localPubs.end()) {
		LOG_DEBUG("Publisher added %s", zPub->getChannelName().c_str());
		_localPubs[zPub->getPort()] = zPub;

		map<string, void*>::iterator sockIter;
		for (sockIter = _sockets.begin(); sockIter != _sockets.end(); sockIter++) {
			zmq_msg_t msg;
			ZMQ_PREPARE(msg, zPub->getChannelName().length() + 5)

			char* buffer = (char*)zmq_msg_data(&msg);
			*(uint16_t*)(buffer) = htons(Message::PUB_ADDED);
			writePubInfo(buffer + 2, zPub->getPort(), zPub->getChannelName().c_str());

			LOG_DEBUG("Informing %s of new publisher", SHORT_UUID(sockIter->first).c_str());
			zmq_sendmsg(sockIter->second, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
		}

	} else {
		assert(zPub->getChannelName().compare(_localPubs[zPub->getPort()]->getChannelName()) == 0);
	}

	assert(validateState());
	_mutex.unlock();
}

void ZeroMQNode::removePublisher(shared_ptr<PublisherImpl> pub) {
	shared_ptr<ZeroMQPublisher> zPub = boost::static_pointer_cast<ZeroMQPublisher>(pub);
	if (zPub.get() == NULL) {
		LOG_ERR("Given publisher cannot be cast to ZeroMQPublisher or is NULL");
		return;
	}

	_mutex.lock();

	if (_localPubs.find(zPub->getPort()) != _localPubs.end()) {
		LOG_DEBUG("Publisher removed %s", zPub->getChannelName().c_str());
		map<string, void*>::iterator sockIter;
		for (sockIter = _sockets.begin(); sockIter != _sockets.end(); sockIter++) {
			zmq_msg_t msg;
			ZMQ_PREPARE(msg, zPub->getChannelName().length() + 5);

			char* buffer = (char*)zmq_msg_data(&msg);
			*(uint16_t*)(buffer) = htons(Message::PUB_REMOVED);
			writePubInfo(buffer + 2, zPub->getPort(), zPub->getChannelName().c_str());

			LOG_DEBUG("Informing %s", sockIter->first.c_str());
			zmq_sendmsg(sockIter->second, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
		}
		_localPubs.erase(zPub->getPort());
	}

	assert(validateState());
	_mutex.unlock();

}

/**
 * Write channel\0port into the given byte array
 */
char* ZeroMQNode::writePubInfo(char* buffer, uint16_t port, const char* channel) {
	char* start = buffer;
	(void)start; // surpress unused warning wiithout assert
	memcpy(buffer, channel, strlen(channel));
	buffer += strlen(channel);
	*buffer = 0;
	buffer++;
	*(uint16_t*)buffer = htons(port);
	buffer += 2;
	assert(buffer - start == (int)strlen(channel) + 3);
	return buffer;
}

/**
 * Read channel\0port from the given byte array into the variables
 */
char* ZeroMQNode::readPubInfo(char* buffer, uint16_t& port, char*& channel) {
	char* start = buffer;
	(void)start; // surpress unused warning wiithout assert
	channel = buffer;
	buffer += strlen(buffer);
	buffer++;
	port = ntohs(*(short*)(buffer));
	buffer += 2;
	assert(buffer - start == (int)strlen(channel) + 3);
	return buffer;
}

std::ostream& operator<<(std::ostream &out, const ZeroMQNode* n) {
	return out;

	out << std::endl;
	out << "ZeroMQNode " << n->_uuid << ": ";
	out << n->_host << ".";
	out << n->_domain << ":";
	out << n->_port;
	out << std::endl << "known nodes:" << std::endl;

	map<string, shared_ptr<NodeStub> >::const_iterator nodeIter;
	for (nodeIter = n->_nodes.begin(); nodeIter != n->_nodes.end(); nodeIter++) {
		out << "\t" << nodeIter->first << ":" << nodeIter->second << std::endl;
	}

	out << std::endl << "known publishers:" << std::endl;

	map<string, map<uint16_t, shared_ptr<PublisherStub> > >::const_iterator nodeIdIter;
	map<uint16_t, shared_ptr<PublisherStub> >::const_iterator pubIter;
	for (nodeIdIter = n->_remotePubs.begin(); nodeIdIter != n->_remotePubs.end(); nodeIdIter++) {
		out << "\t" << nodeIdIter->first << ":" << std::endl;
		for (pubIter = nodeIdIter->second.begin(); pubIter != nodeIdIter->second.end(); pubIter++) {
			out << "\t\t" << pubIter->second->getChannelName() << std::endl;
		}
	}
	out << std::endl;
	return out;
}

bool ZeroMQNode::validateState() {
	_mutex.lock();

	// make sure there is a connection to every node
	map<string, shared_ptr<NodeStub> >::iterator nodeStubIter;
	for (nodeStubIter = _nodes.begin(); nodeStubIter != _nodes.end(); nodeStubIter++) {
		assert(_sockets.find(nodeStubIter->first) != _sockets.end());
	}
	assert(_nodes.size() == _sockets.size());

	// make sure we know the node to every remote publisher
	map<string, map<uint16_t, shared_ptr<PublisherStub> > >::iterator remotePubsIter;
	for (remotePubsIter = _remotePubs.begin(); remotePubsIter != _remotePubs.end(); remotePubsIter++) {
		if (_pendingPubAdditions.find(remotePubsIter->first) != _pendingPubAdditions.end())
			continue;
		assert(_sockets.find(remotePubsIter->first) != _sockets.end());
		assert(_nodes.find(remotePubsIter->first) != _nodes.end());
		shared_ptr<NodeStub> node = _nodes[remotePubsIter->first];

		// make sure node info for remote publishers matches
		map<uint16_t, shared_ptr<PublisherStub> >::iterator remotePubIter;
		for (remotePubIter = remotePubsIter->second.begin(); remotePubIter != remotePubsIter->second.end(); remotePubIter++) {
			shared_ptr<PublisherStub> pub = remotePubIter->second;
			assert(remotePubIter->first == pub->getPort());
			assert(node->getHost().compare(pub->getHost()) == 0);
			assert(node->getDomain().compare(pub->getDomain()) == 0);
			assert(node->getTransport().compare(pub->getTransport()) == 0);
			assert(node->getIP().compare(pub->getIP()) == 0);
		}
	}
	// not every node has publishers
	assert(_remotePubs.size() <= _nodes.size());

	// make sure we know the node for every remote sub
	map<string, map<uint16_t, int > >::iterator remoteSubsIter;
	for (remoteSubsIter = _remoteSubs.begin(); remoteSubsIter != _remoteSubs.end(); remoteSubsIter++) {
    string uuid = remoteSubsIter->first;
		assert(_nodes.find(uuid) != _nodes.end());
		// we should have deleted such a thing
		assert(remoteSubsIter->second.size() > 0);
		map<uint16_t, int>::iterator remoteSubIter;
		for (remoteSubIter = remoteSubsIter->second.begin(); remoteSubIter != remoteSubsIter->second.end(); remoteSubIter++) {

			// make sure the subscriber is connected to an actual local publisher
			assert(_localPubs.find(remoteSubIter->first) != _localPubs.end());

			// we should have deleted such a thing
			assert(remoteSubIter->second > 0);

		}
	}
	_mutex.unlock();
	return true;
}

}

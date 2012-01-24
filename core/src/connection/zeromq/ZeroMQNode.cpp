#include "connection/zeromq/ZeroMQNode.h"

namespace umundo {

shared_ptr<ZeroMQNode> ZeroMQNode::_instance;

shared_ptr<ZeroMQNode> ZeroMQNode::getInstance() {
	if (ZeroMQNode::_instance == NULL) {
		_instance = shared_ptr<ZeroMQNode>(new ZeroMQNode());
		_instance->start();
		// add us as a node
		Discovery::add(_instance);
		// add us as a node query
		Discovery::browse(_instance->_nodeQuery);
	}
	return _instance;
}

ZeroMQNode::~ZeroMQNode() {
	DEBUG_DTOR("ZeroMQNode");
	stop();
	join();
	if (_responder)
		zmq_close(_responder) || LOG_WARN("zmq_close: %s",zmq_strerror(errno));
	if (_zeroMQCtx)
		zmq_term(_zeroMQCtx) || LOG_WARN("zmq_term: %s",zmq_strerror(errno));
}

ZeroMQNode::ZeroMQNode() {
	DEBUG_CTOR("ZeroMQNode");
	(_zeroMQCtx = zmq_init(1)) || LOG_ERR("zmq_init: %s",zmq_strerror(errno));
	(_responder = zmq_socket(_zeroMQCtx, ZMQ_ROUTER))  || LOG_ERR("zmq_socket: %s",zmq_strerror(errno));

	_nodeQuery = new NodeQuery("", this);
	_transport = "tcp";

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
			LOG_WARN("zmq_bind: %s",zmq_strerror(errno))
		}
	}
	_port = port;
	LOG_INFO("Node %s listening as %s", _uuid.c_str(), ss.str().c_str());
}

void ZeroMQNode::run() {
	char* remoteId = NULL;
	int64_t more;
	size_t more_size = sizeof(more);

	while(_isStarted) {
		// read whole envelope
		while (1) {
			zmq_msg_t message;
			zmq_msg_init(&message) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
			zmq_recvmsg(_responder, &message, 0) >= 0 || LOG_WARN("zmq_recvmsg: %s",zmq_strerror(errno))
			int msgSize = zmq_msg_size(&message);

			LOG_DEBUG("ZeroMQNode::run received %d bytes", msgSize);

			if (msgSize == 36 && remoteId == NULL) {
				// remote id from envelope
				remoteId = (char*)malloc(37);
				memcpy(remoteId, zmq_msg_data(&message), 36);
				remoteId[36] = 0;

			} else if (msgSize >= 2) {
				// first two bytes are type of message
				uint16_t type = ntohs(*(short*)(zmq_msg_data(&message)));

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
				}
			}

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
 * We received a publisher from a remote node
 */
void ZeroMQNode::processPubAdded(const char* remoteId, zmq_msg_t message) {
	assert(remoteId != NULL);
	assert(strlen(remoteId) == 36);

	_mutex.lock();

	int msgSize = zmq_msg_size(&message);
	int read = 2; // message type short
	char* pubName = (char*)zmq_msg_data(&message) + read;
	read += strlen(pubName) + 1; // string + terminating zero
	uint16_t port = ntohs(*(short*)((char*)zmq_msg_data(&message) + read));
	read += 2;

	if (read != msgSize) {
		LOG_ERR("Malformed PUB_ADDED received - ignoring");
		_mutex.unlock();
		return;
	}

	shared_ptr<PublisherStub> pubStub = shared_ptr<PublisherStub>(new PublisherStub(pubName));
	pubStub->setPort(port);
	_mutex.unlock();

	addRemotePubToLocalSubs(remoteId, pubStub);
}

void ZeroMQNode::processPubRemoved(const char* remoteId, zmq_msg_t message) {

	assert(remoteId != NULL);
	assert(strlen(remoteId) == 36);

	_mutex.lock();

	int msgSize = zmq_msg_size(&message);
	int read = 2; // message type short
	char* pubName = (char*)zmq_msg_data(&message) + read;
	read += strlen(pubName) + 1; // string + terminating zero
	uint16_t port = ntohs(*(short*)((char*)zmq_msg_data(&message) + read));
	read += 2;

	if (read != msgSize) {
		LOG_ERR("Malformed PUB_REMOVED received - ignoring");
		_mutex.unlock();
		return;
	}
	shared_ptr<PublisherStub> pubStub = _remotePubs[remoteId][port];

	_mutex.unlock();
	if (pubStub)
		removeRemotePubFromLocalSubs(remoteId, pubStub);
}

/**
 * A node was added, connect to its router socket and list our publishers
 */
void ZeroMQNode::added(shared_ptr<NodeStub> node) {
	assert(node);
	assert(node->getUUID().length() == 36);
	if (node->getUUID().compare(_uuid) != 0) {
		// we found ourselves a remote node, lets get some privacy
		_mutex.lock();

		std::stringstream nodeDesc;
		nodeDesc << node << std::endl;
		LOG_INFO("ZeroMQNode::added %s", nodeDesc.str().c_str());

		std::stringstream ss;
		ss << node->getTransport() << "://" << node->getIP() << ":" << node->getPort();
		LOG_DEBUG("ZeroMQNode::added connecting to %s", ss.str().c_str());

		// connect a socket to the remote node port
		void* client;
		(client = zmq_socket(_zeroMQCtx, ZMQ_DEALER)) || LOG_ERR("zmq_socket: %s",zmq_strerror(errno));

		// give the socket an id for zeroMQ routing
		zmq_setsockopt(client, ZMQ_IDENTITY, getUUID().c_str(), getUUID().length()) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));
		zmq_connect(client, ss.str().c_str()) && LOG_WARN("zmq_connect: %s",zmq_strerror(errno));

		// remember node stub and socket
		_nodes[node->getUUID()] = node;
		_sockets[node->getUUID()] = client;

		// send all our local publisher channelnames as "short PUB_ADDED:string CHANNELNAME:short PORT:\0"
		map<uint16_t, ZeroMQPublisher* >::iterator pubIter;
		int hasMore = _localPubs.size() - 1;
		for (pubIter = _localPubs.begin(); pubIter != _localPubs.end(); pubIter++, hasMore--) {

			ZeroMQPublisher* zPub = pubIter->second;
			assert(zPub);

			// create a publisher added message from current publisher
			zmq_msg_t msg;
			initPubMgmtMsg(msg, zPub);
			*(uint16_t*)((char*)zmq_msg_data(&msg) + 0) = htons(Message::PUB_ADDED);

			if (hasMore) {
				zmq_sendmsg(client, &msg, ZMQ_SNDMORE) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			} else {
				zmq_sendmsg(client, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			}
			zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
		}
		_mutex.unlock();
		addRemotePubToLocalSubs(node->getUUID().c_str(), shared_ptr<PublisherStub>());
	}
}

void ZeroMQNode::removed(shared_ptr<NodeStub> node) {
	if (node->getUUID().compare(_uuid) != 0) {
		std::stringstream nodeDesc;
		nodeDesc << node << std::endl;
		LOG_INFO("ZeroMQNode::removed %s", nodeDesc.str().c_str());

		_mutex.lock();

		// unregister local subscribers from remote nodes publishers
		if (_nodes.find(node->getUUID()) != _nodes.end()) {
			map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
			// iterate all remote publishers
			for (pubIter = _remotePubs[node->getUUID()].begin(); pubIter != _remotePubs[node->getUUID()].end(); pubIter++) {
				removeRemotePubFromLocalSubs(node->getUUID().c_str(), pubIter->second);
			}
		}

		zmq_close(_sockets[node->getUUID()]) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));

		_sockets.erase(node->getUUID());               // delete socket
		_remotePubs.erase(node->getUUID());            // remove all references to remote nodes pubs
		_nodes.erase(node->getUUID());                 // remove node itself
		_pendingPubAdditions.erase(node->getUUID());   // I don't know whether this is needed, but it cant be wrong

		assert(_sockets.size() == _remotePubs.size());
		assert(_sockets.size() == _nodes.size());
		_mutex.unlock();

	}
}

void ZeroMQNode::changed(shared_ptr<NodeStub> node) {
	if (node->getUUID().compare(_uuid) != 0) {
		std::stringstream nodeDesc;
		nodeDesc << node << std::endl;
		LOG_INFO("ZeroMQNode::changed %s", nodeDesc.str().c_str());
	}
}

void ZeroMQNode::addRemotePubToLocalSubs(const char* remoteId, shared_ptr<PublisherStub> pub) {
	_mutex.lock();

	// ZeroMQNode::added will call us without a publisher
	if (pub)
		_pendingPubAdditions[remoteId][pub->getPort()] = pub;

	if (_nodes.find(remoteId) != _nodes.end() && _pendingPubAdditions.find(remoteId) != _pendingPubAdditions.end()) {
		// we have discovered the node already
		set<ZeroMQSubscriber* >::iterator subIter;
		map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
		for (subIter = _localSubs.begin(); subIter != _localSubs.end(); subIter++) {
			for (pubIter = _pendingPubAdditions[remoteId].begin(); pubIter != _pendingPubAdditions[remoteId].end(); pubIter++) {

				ZeroMQSubscriber* zSub = *subIter;
				assert(zSub);

				// copy pubStub from pendingPubs to remotePubs
				_remotePubs[remoteId][pubIter->second->getPort()] = pubIter->second;

				// set publisherstub endpoint data from node
				pubIter->second->setNode(_nodes[remoteId]);
				pubIter->second->setHost(_nodes[remoteId]->getHost());
				pubIter->second->setDomain(_nodes[remoteId]->getDomain());
				pubIter->second->setTransport(_nodes[remoteId]->getTransport());
				pubIter->second->setIP(_nodes[remoteId]->getIP());

				if (zSub->getChannelName().compare(pubIter->second->getChannelName()) == 0) {
					zSub->added(pubIter->second);
				}
			}
		}
		_pendingPubAdditions[remoteId].clear();
	}
	_mutex.unlock();
}

void ZeroMQNode::removeRemotePubFromLocalSubs(const char* remoteId, shared_ptr<PublisherStub> pub) {
	_mutex.lock();
	set<ZeroMQSubscriber* >::iterator subIter;
	for (subIter = _localSubs.begin(); subIter != _localSubs.end(); subIter++) {

		ZeroMQSubscriber* zSub = *subIter;
		assert(zSub);

		if (zSub->getChannelName().compare(pub->getChannelName()) == 0) {
			zSub->removed(pub);
		}
	}
	_mutex.unlock();
}

void ZeroMQNode::initPubMgmtMsg(zmq_msg_t& msg, ZeroMQPublisher* pub) {
	zmq_msg_init(&msg) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
	zmq_msg_init_size (&msg, pub->getChannelName().length() + 5) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));

	// initialize message content
	size_t wrote = 0;
	wrote += 2;  // 2 bytes left for PUB_ADDED short
	wrote += snprintf(((char*)zmq_msg_data(&msg) + wrote),
	                  pub->getChannelName().length() + 1,
	                  "%s",
	                  pub->getChannelName().c_str());
	wrote++; // string terminating zero
	*(uint16_t*)((char*)zmq_msg_data(&msg) + pub->getChannelName().length() + 3) = htons(pub->getPort());
	wrote += 2;
	assert(wrote == zmq_msg_size(&msg));
}

/**
 * Add a local subscriber.
 *
 * This will call added(pub) for every known publisher with a matching channelname.
 */
void ZeroMQNode::addSubscriber(SubscriberImpl* sub) {
	shared_ptr<ZeroMQNode> myself = getInstance();
	ZeroMQSubscriber* zSub = (ZeroMQSubscriber*)(sub);
	if (zSub == NULL)
		return;

	myself->_mutex.lock();

	myself->_localSubs.insert(zSub);
	map<string, map<uint16_t, shared_ptr<PublisherStub> > >::iterator nodeIdIter;
	map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
	for (nodeIdIter = myself->_remotePubs.begin(); nodeIdIter != myself->_remotePubs.end(); nodeIdIter++) {
		for (pubIter = myself->_remotePubs[nodeIdIter->first].begin(); pubIter != myself->_remotePubs[nodeIdIter->first].end(); pubIter++) {
			if (zSub->getChannelName().compare(pubIter->second->getChannelName()) == 0) {
				zSub->added(pubIter->second);
			}
		}
	}
	myself->_mutex.unlock();
}

/**
 * Remove a local subscriber.
 *
 * This will call removed(pub) for every publisher with a matching channelname.
 */
void ZeroMQNode::removeSubscriber(SubscriberImpl* sub) {
	shared_ptr<ZeroMQNode> myself = getInstance();
	ZeroMQSubscriber* zSub = (ZeroMQSubscriber*)(sub);
	if (zSub == NULL)
		return;

	// do we know this subscriber?
	if (myself->_localSubs.find(zSub) == myself->_localSubs.end())
		return;

	myself->_mutex.lock();
	// disconnect all publishers
	map<string, map<uint16_t, shared_ptr<PublisherStub> > >::iterator nodeIdIter;
	map<uint16_t, shared_ptr<PublisherStub> >::iterator pubIter;
	for (nodeIdIter = myself->_remotePubs.begin(); nodeIdIter != myself->_remotePubs.end(); nodeIdIter++) {
		for (pubIter = myself->_remotePubs[nodeIdIter->first].begin(); pubIter != myself->_remotePubs[nodeIdIter->first].end(); pubIter++) {
			if (zSub->getChannelName().compare(pubIter->second->getChannelName()) == 0) {
				zSub->removed(pubIter->second);
			}
		}
	}
	myself->_mutex.unlock();

}

void ZeroMQNode::addPublisher(PublisherImpl* pub) {
	shared_ptr<ZeroMQNode> myself = getInstance();
	ZeroMQPublisher* zPub = (ZeroMQPublisher*)(pub);
	if (zPub == NULL) {
		LOG_WARN("Given publisher cannot be cast to ZeroMQPublisher or is NULL");
		return;
	}

	myself->_mutex.lock();
	// do we already now this publisher?
	if (myself->_localPubs.find(zPub->getPort()) == myself->_localPubs.end()) {
		LOG_DEBUG("Publisher added %s", zPub->getChannelName().c_str());
		myself->_localPubs[zPub->getPort()] = zPub;

		map<string, void*>::iterator sockIter;
		for (sockIter = myself->_sockets.begin(); sockIter != myself->_sockets.end(); sockIter++) {
			zmq_msg_t msg;
			initPubMgmtMsg(msg, zPub);
			*(uint16_t*)((char*)zmq_msg_data(&msg) + 0) = htons(Message::PUB_ADDED);
			LOG_DEBUG("Informing %s", sockIter->first.c_str());
			zmq_sendmsg(sockIter->second, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
		}

	} else {
		assert(zPub->getChannelName().compare(myself->_localPubs[zPub->getPort()]->getChannelName()) == 0);
	}

	myself->_mutex.unlock();
}

void ZeroMQNode::removePublisher(PublisherImpl* pub) {
	shared_ptr<ZeroMQNode> myself = getInstance();
	ZeroMQPublisher* zPub = (ZeroMQPublisher*)(pub);
	if (zPub == NULL) {
		LOG_WARN("Given publisher cannot be cast to ZeroMQPublisher or is NULL");
		return;
	}

	myself->_mutex.lock();

	if (myself->_localPubs.find(zPub->getPort()) != myself->_localPubs.end()) {
		LOG_DEBUG("Publisher removed %s", zPub->getChannelName().c_str());
		map<string, void*>::iterator sockIter;
		for (sockIter = myself->_sockets.begin(); sockIter != myself->_sockets.end(); sockIter++) {
			zmq_msg_t msg;
			initPubMgmtMsg(msg, zPub);
			*(uint16_t*)((char*)zmq_msg_data(&msg) + 0) = htons(Message::PUB_REMOVED);
			LOG_DEBUG("Informing %s", sockIter->first.c_str());
			zmq_sendmsg(sockIter->second, &msg, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
			zmq_msg_close(&msg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
		}
		myself->_localPubs.erase(zPub->getPort());
	}

	myself->_mutex.unlock();

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

}
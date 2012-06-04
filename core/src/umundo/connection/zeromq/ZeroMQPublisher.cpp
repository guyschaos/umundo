#ifdef WIN32
#include <time.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

#include "umundo/connection/zeromq/ZeroMQPublisher.h"

#include "umundo/connection/zeromq/ZeroMQNode.h"
#include "umundo/common/Message.h"
#include "umundo/common/UUID.h"

#include "umundo/config.h"
#if defined UNIX || defined IOS || defined IOSSIM
#include <string.h> // strlen, memcpy
#include <stdio.h> // snprintf
#endif

namespace umundo {

shared_ptr<Implementation> ZeroMQPublisher::create(void* facade) {
	shared_ptr<Implementation> instance(new ZeroMQPublisher());
	boost::static_pointer_cast<ZeroMQPublisher>(instance)->_facade = facade;
	return instance;
}

void ZeroMQPublisher::destroy() {
	delete(this);
}

void ZeroMQPublisher::init(shared_ptr<Configuration> config) {

	_uuid = (_uuid.length() > 0 ? _uuid : UUID::getUUID());
	_config = boost::static_pointer_cast<PublisherConfig>(config);
	_transport = "tcp";
	_pubCount = 0;
	(_socket = zmq_socket(ZeroMQNode::getZeroMQContext(), ZMQ_PUB)) || LOG_WARN("zmq_socket: %s",zmq_strerror(errno));

	int hwm = NET_ZEROMQ_SND_HWM;
	zmq_setsockopt(_socket, ZMQ_SNDHWM, &hwm, sizeof(hwm)) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));

	std::stringstream ssIpc;
	ssIpc << "ipc:///tmp/" << _uuid;
	zmq_bind(_socket, ssIpc.str().c_str()) && LOG_WARN("zmq_bind: %s",zmq_strerror(errno));

	std::stringstream ssInProc;
	ssInProc << "inproc://" << _uuid;
	zmq_bind(_socket, ssInProc.str().c_str()) && LOG_WARN("zmq_bind: %s",zmq_strerror(errno));

	uint16_t port = 4242;
	std::stringstream ssNet;
	ssNet << _transport << "://*:" << port;

	while(zmq_bind(_socket, ssNet.str().c_str()) < 0) {
		switch(errno) {
		case EADDRINUSE:
			port++;
			ssNet.clear();             // clear error bits
			ssNet.str(string());  // reset string
			ssNet << _transport << "://*:" << port;
			break;
		default:
			LOG_WARN("zmq_bind: %s",zmq_strerror(errno));
			Thread::sleepMs(100);
		}
	}
	_port = port;

	LOG_DEBUG("ZeroMQPublisher bound to %s", ssNet.str().c_str());
	LOG_DEBUG("ZeroMQPublisher bound to %s", ssIpc.str().c_str());
	LOG_DEBUG("ZeroMQPublisher bound to %s", ssInProc.str().c_str());
}

ZeroMQPublisher::ZeroMQPublisher() {
}

ZeroMQPublisher::~ZeroMQPublisher() {
	zmq_close(_socket) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));

	std::stringstream ssIpc;
	ssIpc << "/tmp/" << _uuid;

	remove(ssIpc.str().c_str());
	//zmq_term(_zeroMQCtx) && LOG_WARN("zmq_term: %s",zmq_strerror(errno));
}

void ZeroMQPublisher::suspend() {
	if (_isSuspended)
		return;
	_isSuspended = true;
	zmq_close(_socket) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));
}

void ZeroMQPublisher::resume() {
	if (!_isSuspended)
		return;
	_isSuspended = false;
	init(_config);
}

/**
 * Block until we have a given number of subscribers.
 */
int ZeroMQPublisher::waitForSubscribers(int count) {
	while (_pubCount < count) {
		UMUNDO_WAIT(_pubLock);
		// give the connection a moment to establish
		Thread::sleepMs(300);
	}
	return _pubCount;
}

void ZeroMQPublisher::addedSubscriber(const string remoteId, const string subId) {
	_pubCount++;
	UMUNDO_SIGNAL(_pubLock);
	if (_greeter != NULL)
		_greeter->welcome((Publisher*)_facade, remoteId, subId);
}

void ZeroMQPublisher::removedSubscriber(const string remoteId, const string subId) {
	_pubCount--;
	UMUNDO_SIGNAL(_pubLock);
	if (_greeter != NULL)
		_greeter->farewell((Publisher*)_facade, remoteId, subId);
}

void ZeroMQPublisher::send(Message* msg) {
	//LOG_DEBUG("ZeroMQPublisher sending msg on %s", _channelName.c_str());
	if (_isSuspended) {
		LOG_WARN("Not sending message on suspended publisher");
		return;
	}

	// topic name or explicit subscriber id is first message in envelope
	zmq_msg_t channelEnvlp;
	if (msg->getMeta().find("subscriber") != msg->getMeta().end()) {
		// explicit destination
		ZMQ_PREPARE_STRING(channelEnvlp, msg->getMeta("subscriber").c_str(), msg->getMeta("subscriber").size());
	} else {
		// everyone on channel
		ZMQ_PREPARE_STRING(channelEnvlp, _channelName.c_str(), _channelName.size());
	}
	zmq_sendmsg(_socket, &channelEnvlp, ZMQ_SNDMORE) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&channelEnvlp) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));

	// mandatory meta fields
	msg->setMeta("publisher", _uuid);
	msg->setMeta("proc", procUUID);

	// all our meta information
	map<string, string>::const_iterator metaIter;
	for (metaIter = msg->getMeta().begin(); metaIter != msg->getMeta().end(); metaIter++) {
		// string key(metaIter->first);
		// string value(metaIter->second);
		// std::cout << key << ": " << value << std::endl;

		// string length of key + value + two null bytes as string delimiters
		size_t metaSize = (metaIter->first).length() + (metaIter->second).length() + 2;
		zmq_msg_t metaMsg;
		ZMQ_PREPARE(metaMsg, metaSize);

		char* writePtr = (char*)zmq_msg_data(&metaMsg);

		memcpy(writePtr, (metaIter->first).data(), (metaIter->first).length());
		// indexes start at zero, so length is the byte after the string
		((char*)zmq_msg_data(&metaMsg))[(metaIter->first).length()] = '\0';
		assert(strlen((char*)zmq_msg_data(&metaMsg)) == (metaIter->first).length());
		assert(strlen(writePtr) == (metaIter->first).length()); // just to be sure

		// increment write pointer
		writePtr += (metaIter->first).length() + 1;

		memcpy(writePtr,
		       (metaIter->second).data(),
		       (metaIter->second).length());
		// first string + null byte + second string
		((char*)zmq_msg_data(&metaMsg))[(metaIter->first).length() + 1 + (metaIter->second).length()] = '\0';
		assert(strlen(writePtr) == (metaIter->second).length());

		zmq_sendmsg(_socket, &metaMsg, ZMQ_SNDMORE) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
		zmq_msg_close(&metaMsg) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));

	}

	// data as the second part of a multipart message
	zmq_msg_t publication;
	ZMQ_PREPARE_DATA(publication, msg->data(), msg->size());
	zmq_sendmsg(_socket, &publication, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&publication) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
}


}
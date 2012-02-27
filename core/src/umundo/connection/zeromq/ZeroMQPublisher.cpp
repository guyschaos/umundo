#ifdef WIN32
#include <time.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

#include "umundo/connection/zeromq/ZeroMQPublisher.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>


#include "umundo/connection/zeromq/ZeroMQNode.h"
#include "umundo/common/Message.h"

#include "umundo/config.h"
#if defined UNIX || defined IOS || defined IOSSIM
#include <string.h> // strlen, memcpy
#include <stdio.h> // snprintf
#endif

namespace umundo {

shared_ptr<Implementation> ZeroMQPublisher::create() {
	shared_ptr<Implementation> instance(new ZeroMQPublisher());
	return instance;
}

void ZeroMQPublisher::destroy() {
	delete(this);
}

void ZeroMQPublisher::init(shared_ptr<Configuration> config) {

	_uuid = boost::lexical_cast<string>(boost::uuids::random_generator()());
	_config = boost::static_pointer_cast<PublisherConfig>(config);
	_transport = "tcp";
  _pubCount = 0;
  _pubLock.lock();
	(_socket = zmq_socket(ZeroMQNode::getZeroMQContext(), ZMQ_XPUB)) || LOG_WARN("zmq_socket: %s",zmq_strerror(errno));

  int hwm = NET_ZEROMQ_SND_HWM;
	zmq_setsockopt(_socket, ZMQ_SNDHWM, &hwm, sizeof(hwm)) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));

	uint16_t port = 4242;

	std::stringstream ss;
	ss << _transport << "://*:" << port;

	while(zmq_bind(_socket, ss.str().c_str()) < 0) {
		switch(errno) {
		case EADDRINUSE:
			port++;
			ss.clear();             // clear error bits
			ss.str(string());  // reset string
			ss << _transport << "://*:" << port;
			break;
		default:
			LOG_WARN("zmq_bind: %s",zmq_strerror(errno));
      Thread::sleepMs(100);
		}
	}
	_port = port;

	LOG_DEBUG("ZeroMQPublisher bound to %s", ss.str().c_str());
}

ZeroMQPublisher::ZeroMQPublisher() {
	DEBUG_CTOR("ZeroMQPublisher");
}

ZeroMQPublisher::~ZeroMQPublisher() {
	DEBUG_DTOR("ZeroMQPublisher start");
	zmq_close(_socket) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));
	//zmq_term(_zeroMQCtx) && LOG_WARN("zmq_term: %s",zmq_strerror(errno));
	DEBUG_DTOR("ZeroMQPublisher finished");
}

/**
 * Block until we have a given number of subscribers.
 */
int ZeroMQPublisher::waitForSubscribers(int count) {
  while (_pubCount < count) {
    _pubLock.lock();
    _pubLock.unlock();
    // give the connection a moment to establish
    Thread::sleepMs(100);
  }
  return _pubCount;
}

void ZeroMQPublisher::addedSubscriber() {
  _pubCount++;
  _pubLock.unlock();
  Thread::yield();
  _pubLock.lock();
}

void ZeroMQPublisher::removedSubscriber() {
  _pubCount--;
  _pubLock.unlock();
  Thread::yield();
  _pubLock.lock();
}

void ZeroMQPublisher::send(Message* msg) {
	//LOG_DEBUG("ZeroMQPublisher sending msg on %s", _channelName.c_str());

	// topic name as envelope
	zmq_msg_t channelEnvlp;
	zmq_msg_init(&channelEnvlp) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
	zmq_msg_init_size (&channelEnvlp, _channelName.size() + 1) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));
	memcpy(zmq_msg_data(&channelEnvlp), _channelName.c_str(), _channelName.size());
	((char*)zmq_msg_data(&channelEnvlp))[_channelName.size()] = '\0';
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
		zmq_msg_init(&metaMsg) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
		zmq_msg_init_size (&metaMsg, metaSize) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));

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
	zmq_msg_init(&publication) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
	zmq_msg_init_size (&publication, msg->getData().size()) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));
	memcpy(zmq_msg_data(&publication), msg->getData().data(), msg->getData().size());
	zmq_sendmsg(_socket, &publication, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&publication) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
}


}
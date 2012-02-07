#include "connection/zeromq/ZeroMQPublisher.h"

#include "connection/zeromq/ZeroMQNode.h"
#include "common/Message.h"

#include "config.h"
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

	_config = boost::static_pointer_cast<PublisherConfig>(config);
	_transport = "tcp";
	(_socket = zmq_socket(ZeroMQNode::getZeroMQContext(), ZMQ_PUB)) || LOG_WARN("zmq_socket: %s",zmq_strerror(errno));

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
	zmq_term(_zeroMQCtx) && LOG_WARN("zmq_term: %s",zmq_strerror(errno));
	DEBUG_DTOR("ZeroMQPublisher finished");
}

void ZeroMQPublisher::send(Message* msg) {
	//LOG_DEBUG("ZeroMQPublisher sending msg on %s", _channelName.c_str());

	// topic name as envelope
	zmq_msg_t channelEnvlp;
	zmq_msg_init(&channelEnvlp) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
	zmq_msg_init_size (&channelEnvlp, _channelName.size()) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));
	memcpy(zmq_msg_data(&channelEnvlp), _channelName.c_str(), _channelName.size());
	zmq_sendmsg(_socket, &channelEnvlp, ZMQ_SNDMORE) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&channelEnvlp) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));

	// all our meta information
	map<string, string>::const_iterator metaIter;
	for (metaIter = msg->getMeta().begin(); metaIter != msg->getMeta().end(); metaIter++) {
		size_t metaSize = (metaIter->first).length() + (metaIter->second).length() + 2;
		zmq_msg_t metaMsg;
		zmq_msg_init(&metaMsg) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
		zmq_msg_init_size (&metaMsg, metaSize) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));
		snprintf((char*)zmq_msg_data(&metaMsg), (metaIter->first).length() + 1, "%s", (metaIter->first).data());
		snprintf(
             (char*)zmq_msg_data(&metaMsg) + (metaIter->first).length() + 1, 
             (metaIter->second).length() + 1, 
             "%s", 
             (metaIter->second).data());
		
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
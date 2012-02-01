#include "connection/zeromq/ZeroMQPublisher.h"
#include "connection/zeromq/ZeroMQNode.h"

namespace umundo {

PublisherImpl* ZeroMQPublisher::create(string channelName) {
	PublisherImpl* instance = new ZeroMQPublisher(channelName);
	ZeroMQNode::addPublisher(instance);
	return instance;
}

ZeroMQPublisher::ZeroMQPublisher(string channelname) : PublisherImpl(channelname) {
	DEBUG_CTOR("ZeroMQPublisher");

	_transport = "tcp";
	(_zeroMQCtx = zmq_init(1)) || LOG_WARN("zmq_init: %s",zmq_strerror(errno));
	(_socket = zmq_socket(_zeroMQCtx, ZMQ_PUB)) || LOG_WARN("zmq_socket: %s",zmq_strerror(errno));
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
//	int64_t hwm = ZEROMQ_PUB_HWM;
//	zmq_setsockopt(_socket, ZMQ_HWM, &hwm, sizeof (hwm)) && ZMQWARN("zmq_setsockopt");

	LOG_DEBUG("ZeroMQPublisher bound to %s", ss.str().c_str());

}

ZeroMQPublisher::~ZeroMQPublisher() {
	DEBUG_DTOR("ZeroMQPublisher start");
	ZeroMQNode::removePublisher(this);
	zmq_close(_socket) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));
	zmq_term(_zeroMQCtx) && LOG_WARN("zmq_term: %s",zmq_strerror(errno));
	DEBUG_DTOR("ZeroMQPublisher finished");
}

uint16_t ZeroMQPublisher::getPort() {
	return _port;
}

void ZeroMQPublisher::send(char* buffer, size_t length) {
	LOG_DEBUG("ZeroMQPublisher sending %d bytes on %s", length, _channelName.c_str());

	// topic name as envelope
	zmq_msg_t channelEnvlp;
	zmq_msg_init(&channelEnvlp) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
	zmq_msg_init_size (&channelEnvlp, _channelName.size()) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));
	memcpy(zmq_msg_data(&channelEnvlp), _channelName.c_str(), _channelName.size());
	zmq_sendmsg(_socket, &channelEnvlp, ZMQ_SNDMORE) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&channelEnvlp) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));

	// data as the second part of a multipart message
	zmq_msg_t publication;
	zmq_msg_init(&publication) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));
	zmq_msg_init_size (&publication, length) && LOG_WARN("zmq_msg_init_size: %s",zmq_strerror(errno));
	memcpy(zmq_msg_data(&publication), buffer, length);
	zmq_sendmsg(_socket, &publication, 0) >= 0 || LOG_WARN("zmq_sendmsg: %s",zmq_strerror(errno));
	zmq_msg_close(&publication) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
}


}
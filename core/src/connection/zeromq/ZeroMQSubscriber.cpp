#include "connection/zeromq/ZeroMQSubscriber.h"
#include "connection/zeromq/ZeroMQNode.h"

namespace umundo {

SubscriberImpl* ZeroMQSubscriber::create(string channelName, Receiver* recv) {
	SubscriberImpl* instance = new ZeroMQSubscriber(channelName, recv);
	ZeroMQNode::addSubscriber(instance);
	instance->start();
	return instance;
}

ZeroMQSubscriber::ZeroMQSubscriber() {
	DEBUG_CTOR("ZeroMQSubscriber");
}

ZeroMQSubscriber::ZeroMQSubscriber(string channelName, Receiver* receiver) : _channelName(channelName), _receiver(receiver) {
	DEBUG_CTOR("ZeroMQSubscriber");
	(_zeroMQCtx = zmq_init(1)) || LOG_WARN("zmq_init: %s",zmq_strerror(errno));
	(_socket = zmq_socket(_zeroMQCtx, ZMQ_SUB)) || LOG_WARN("zmq_socket: %s",zmq_strerror(errno));

	std::string _uuid = boost::lexical_cast<string>(boost::uuids::random_generator()());
	zmq_setsockopt(_socket, ZMQ_SUBSCRIBE, _channelName.c_str(), _channelName.size()) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));
	zmq_setsockopt (_socket, ZMQ_IDENTITY, _uuid.c_str(), _uuid.length()) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));
}

ZeroMQSubscriber::~ZeroMQSubscriber() {
	DEBUG_DTOR("ZeroMQSubscriber start");
	ZeroMQNode::removeSubscriber(this);
	zmq_close(_socket) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));
	zmq_term(_zeroMQCtx) && LOG_WARN("zmq_term: %s",zmq_strerror(errno));
	DEBUG_DTOR("ZeroMQSubscriber finished");
}

const string& ZeroMQSubscriber::getChannelName() {
	return _channelName;
}

void ZeroMQSubscriber::run() {
	int64_t more;
	size_t more_size = sizeof(more);

	while(isStarted()) {
		// read whole envelope
		while (1) {
			zmq_msg_t message;
			zmq_msg_init(&message) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));

//      std::cout << "subscriber trying to read from socket" << std::endl;
			zmq_recvmsg(_socket, &message, 0) >= 0 || LOG_WARN("zmq_recvmsg: %s",zmq_strerror(errno));
			int msgSize = zmq_msg_size(&message);

			// send data to our receiver
			_receiver->receive((char*)zmq_msg_data(&message), msgSize);
			zmq_getsockopt(_socket, ZMQ_RCVMORE, &more, &more_size) && LOG_WARN("zmq_getsockopt: %s",zmq_strerror(errno));
			zmq_msg_close(&message) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));

			// std::cout << "received " << msgSize << "bytes:" << std::endl;
			// for (int i = 0; i < msgSize; i++) {
			//   std::cout << (int)((char*)zmq_msg_data(&message))[i] << ":";
			// }
			// std::cout << std::endl;

			if (!more) {
				break; // last message part
			}
		}
	}
}

void ZeroMQSubscriber::added(shared_ptr<PublisherStub> pub) {
	std::stringstream ss;
	ss << pub->getTransport() << "://" << pub->getIP() << ":" << pub->getPort();
	LOG_DEBUG("ZeroMQSubscriber connecting to %s", ss.str().c_str());
	zmq_connect(_socket, ss.str().c_str()) && LOG_WARN("zmq_connect: %s",zmq_strerror(errno));
}

void ZeroMQSubscriber::removed(shared_ptr<PublisherStub> pub) {
	// there is nothing to do here .. zeroMQ does not support "disconnect"
	std::stringstream ss;
	ss << pub->getTransport() << "://" << pub->getIP() << ":" << pub->getPort();
	LOG_DEBUG("ZeroMQSubscriber disconnecting from %s", ss.str().c_str());
}

void ZeroMQSubscriber::changed(shared_ptr<PublisherStub>) {
	// never called
}


}
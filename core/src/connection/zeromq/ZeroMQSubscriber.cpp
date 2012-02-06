#include "connection/zeromq/ZeroMQSubscriber.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "connection/zeromq/ZeroMQNode.h"
#include "connection/Publisher.h"
#include "common/Message.h"

namespace umundo {

shared_ptr<Implementation> ZeroMQSubscriber::create() {
	shared_ptr<Implementation> instance(new ZeroMQSubscriber());
	return instance;
}

void ZeroMQSubscriber::destroy() {
	delete(this);
}

ZeroMQSubscriber::ZeroMQSubscriber() {
	DEBUG_CTOR("ZeroMQSubscriber");
}

ZeroMQSubscriber::~ZeroMQSubscriber() {
	DEBUG_DTOR("ZeroMQSubscriber start");
	zmq_close(_socket) && LOG_WARN("zmq_close: %s",zmq_strerror(errno));
	zmq_term(_zeroMQCtx) && LOG_WARN("zmq_term: %s",zmq_strerror(errno));
	DEBUG_DTOR("ZeroMQSubscriber finished");
}

void ZeroMQSubscriber::init(shared_ptr<Configuration> config) {
	_config = boost::static_pointer_cast<SubscriberConfig>(config);
	(_socket = zmq_socket(ZeroMQNode::getZeroMQContext(), ZMQ_SUB)) || LOG_WARN("zmq_socket: %s",zmq_strerror(errno));

	_uuid = boost::lexical_cast<string>(boost::uuids::random_generator()());
	zmq_setsockopt(_socket, ZMQ_SUBSCRIBE, _channelName.c_str(), _channelName.size()) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));
	zmq_setsockopt (_socket, ZMQ_IDENTITY, _uuid.c_str(), _uuid.length()) && LOG_WARN("zmq_setsockopt: %s",zmq_strerror(errno));

	start();
}

void ZeroMQSubscriber::run() {
	int64_t more;
	size_t more_size = sizeof(more);

	while(isStarted()) {
		// read whole envelope
		Message* msg = new Message();
		while (1) {
			zmq_msg_t message;
			zmq_msg_init(&message) && LOG_WARN("zmq_msg_init: %s",zmq_strerror(errno));

			zmq_recvmsg(_socket, &message, 0) >= 0 || LOG_WARN("zmq_recvmsg: %s",zmq_strerror(errno));
			size_t msgSize = zmq_msg_size(&message);

			// last message contains actual data
			zmq_getsockopt(_socket, ZMQ_RCVMORE, &more, &more_size) && LOG_WARN("zmq_getsockopt: %s",zmq_strerror(errno));

			if (more) {
				char* key = (char*)zmq_msg_data(&message);
				char* value = ((char*)zmq_msg_data(&message) + strlen(key) + 1);
        zmq_msg_close(&message) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
        
        // is this the first message with the channelname?
        if (strlen(value) == 0 && 
            strlen(key) + 1 == msgSize &&
            msg->getMeta().find(key) == msg->getMeta().end()) {
          msg->setMeta("channelName", key);
        } else {
          if (strlen(key) + strlen(value) + 2 != msgSize) {
            LOG_INFO("Received malformed message");
            break;
          } else {
            msg->setMeta(key, value);
          }
        }
        
			} else {
				msg->setData(string((char*)zmq_msg_data(&message), msgSize));
        zmq_msg_close(&message) && LOG_WARN("zmq_msg_close: %s",zmq_strerror(errno));
				_receiver->receive(msg);
				delete(msg);
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
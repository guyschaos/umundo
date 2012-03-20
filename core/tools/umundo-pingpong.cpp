#include "umundo/core.h"
#include <iostream>
#include <string.h>

using namespace umundo;

class TestReceiver : public Receiver {
public:
	TestReceiver() {};
	void receive(Message* msg) {
		std::cout << "i" << std::flush;
	}
};

int main(int argc, char** argv) {
	TestReceiver* testRecv = new TestReceiver();
	Publisher* pubFoo = new Publisher("pingpong");
	Subscriber* subFoo = new Subscriber("pingpong", testRecv);

	Node* mainNode = new Node();
	mainNode->addPublisher(pubFoo);
	mainNode->addSubscriber(subFoo);

	while(1) {
		Thread::sleepMs(1000);
		Message* msg = new Message();
		msg->setData("ping");
		std::cout << "o" << std::flush;
		pubFoo->send(msg);
		delete(msg);
	}
}

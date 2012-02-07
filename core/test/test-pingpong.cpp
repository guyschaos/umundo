#include "umundo/core.h"
#include <iostream>

using namespace umundo;

class TestReceiver : public Receiver {
public:
	TestReceiver() {};
	void receive(Message* msg) {
		std::cout << "Received " << msg->getData() << std::endl;
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
    Thread::sleepMs(500);
    Message* msg = new Message();
    msg->setData("ping");
    pubFoo->send(msg);
  }
}

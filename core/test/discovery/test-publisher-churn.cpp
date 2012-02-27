#include <iostream>
#include "umundo/core.h"
#include <stdio.h>

using namespace umundo;

class TestReceiver : public Receiver {
public:
	std::string _name;
	TestReceiver(std::string name) : _name(name) {};
	void receive(Message* msg) {
		std::cout << _name << " received " << msg->getData().size() << "bytes" << std::endl;
	}	
};

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
  TestReceiver *testRecv = new TestReceiver("recv1");
	Node* node = new Node();
  Subscriber *subFoo = new Subscriber("fooChannel", testRecv);
	node->addSubscriber(subFoo);

  char buffer[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (char)i%255;
  }
 	
  // create and destroy publishers
  while(1) {
    {
      boost::shared_ptr<Publisher> pubFoo = boost::shared_ptr<Publisher>(new Publisher("fooChannel"));
      pubFoo->send(buffer, BUFFER_SIZE);
      getchar();
    }
  }
}

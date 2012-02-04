#include "discovery/Discovery.h"
#include "connection/Publisher.h"
#include "connection/Subscriber.h"
#include "common/Node.h"
#include "common/stdInc.h"

#include <iostream>

using namespace umundo;
using boost::shared_ptr;

class TestReceiver : public Receiver {
public:
	std::string _name;
	TestReceiver(std::string name) : _name(name) {};
	void receive(char* buffer, size_t length) {
		std::cout << _name << " received " << length << "bytes:" << std::endl << buffer << std::endl;
	}	
};

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
  TestReceiver* testRecv = new TestReceiver("recv1");
  Publisher* pubFoo = new Publisher("fooChannel");
  Subscriber* subFoo = new Subscriber("fooChannel", testRecv);

  Node* mainNode = new Node();
  mainNode->addPublisher(pubFoo);
  mainNode->addSubscriber(subFoo);
  
  char buffer[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (char)i%255;
  }
	
  while(1) {
    Thread::sleepMs(1000);
    pubFoo->send(buffer, BUFFER_SIZE);
  }
}

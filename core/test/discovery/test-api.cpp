#include "discovery/Discovery.h"
#include "connection/Publisher.h"
#include "connection/Subscriber.h"
#include "common/Message.h"
#include "common/Node.h"

#include <iostream>

#ifdef __GNUC__
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}
#endif


using namespace umundo;
using boost::shared_ptr;

class TestReceiver : public Receiver {
public:
	std::string _name;
	TestReceiver(std::string name) : _name(name) {};
	void receive(Message* msg) {
		std::cout << _name << " received " << msg->getData().size() << "bytes:" << std::endl << msg->getData() << std::endl;
	}	
};

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
#ifdef __GNUC__
	signal(SIGSEGV, handler);   // install our handler
#endif
	
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
    Message* msg = new Message();
    msg->setData(string(buffer, BUFFER_SIZE));
    msg->setMeta("type", "foo!");
    pubFoo->send(msg);
  }
}

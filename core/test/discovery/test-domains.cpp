#include "umundo/core.h"

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

static int receives = 0;
static Mutex mutex;

class TestReceiver : public Receiver {
public:
	std::string _name;
	TestReceiver(std::string name) : _name(name) {};
	void receive(Message* msg) {
    //std::cout << "received " << msg->getData().size() << "bytes" << std::endl;
    assert(_name.compare(msg->getMeta("channelName")) == 0);
    mutex.lock();
    // not an atomic operation!
    receives++;
    mutex.unlock();
	}	
};

#define BUFFER_SIZE 1024*1024

int main(int argc, char** argv) {
#ifdef __GNUC__
	signal(SIGSEGV, handler);   // install our handler
#endif
	
  Subscriber* test1Sub = new Subscriber("test1", new TestReceiver("test1"));
  Publisher* test1Pub = new Publisher("test1");

	/**
	 * Start two nodes in two different domains and add the test1 publisher
	 */

	// A node in the fooDomain with our publisher
  Node* test1Node = new Node("fooDomain");
  test1Node->addPublisher(test1Pub);
  // A node in the barDomain with our publisher
  Node* test2Node = new Node("barDomain");
  test2Node->addPublisher(test1Pub);
	
  // yet another node where none is subscribed to
  Node* test5Node = new Node("unsubscribedDomain");
  test5Node->addPublisher(test1Pub);

	/**
	 * Start two more nodes in those domains and add the test1 subscriber
	 */
	
	// Another node in the fooDomain with a publisher
  Node* test3Node = new Node("fooDomain");
  test3Node->addSubscriber(test1Sub);

	// Another node in the fooDomain with a publisher
  Node* test4Node = new Node("barDomain");
  test4Node->addSubscriber(test1Sub);

  // yet another node where none will publish
  Node* test6Node = new Node("unpublishedDomain");
  test6Node->addSubscriber(test1Sub);

  char buffer[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (char)i%255;
  }

  // get connected
  Thread::sleepMs(1500);
  int iterations = 500; // this has to be less or equal to the high water mark
  for (int i = 0; i < iterations; i++) {
    Message* msg = new Message();
    msg->setData(string(buffer, BUFFER_SIZE));
    msg->setMeta("type", "foo!");
    test1Pub->send(msg);
  }
  // yield to give subscriber a chance to receive
  Thread::sleepMs(200);
  std::cout << "Received " << receives << " messages, expected " << 2 * iterations << " messages" << std::endl;
  assert(receives == 2 * iterations);
}

#include "umundo/core.h"
#include <iostream>

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

void testDifferentDomain() {
	Node* fooNode = new Node("foo");
  Node* barNode = new Node("bar");

  Subscriber* sub = new Subscriber("test1", new TestReceiver("test1"));
  Publisher* pub = new Publisher("test1");

	fooNode->addPublisher(pub);
	barNode->addSubscriber(sub);
	Thread::sleepMs(2000);
	assert(pub->waitForSubscribers(0) == 0);
	
	delete(fooNode);
	delete(barNode);
}

void testSameDomain() {
	Node* fooNode = new Node("foo");
  Node* barNode = new Node("foo");

  Subscriber* sub = new Subscriber("test1", new TestReceiver("test1"));
  Publisher* pub = new Publisher("test1");

	fooNode->addPublisher(pub);
	barNode->addSubscriber(sub);
	Thread::sleepMs(2000);
	assert(pub->waitForSubscribers(1) == 1);
	
	delete(fooNode);
	delete(barNode);
}

#define BUFFER_SIZE 1024*1024

int main(int argc, char** argv, char** envp) {	
	
	testDifferentDomain();
	testSameDomain();
	return EXIT_SUCCESS;

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
//    test1Pub->send(msg);
  }
  // yield to give subscriber a chance to receive
  Thread::sleepMs(200);
  std::cout << "Received " << receives << " messages, expected " << 2 * iterations << " messages" << std::endl;
//  assert(receives == 2 * iterations);
}

#include "umundo/core.h"
#include "umundo/util.h"
#include <iostream>
#include <stdio.h>

#define BUFFER_SIZE 1024*1024

using namespace umundo;

static int nrReceptions = 0;
static int bytesRecvd = 0;


class TestReceiver : public Receiver {
	void receive(Message* msg) {
    std::cout << "md5: '" << msg->getMeta("md5") << "'" << std::endl;
    std::cout << "md5: '" << md5(msg->data(), msg->size()) << "'" << std::endl;
    std::cout << "equals: " << msg->getMeta("md5").compare(md5(msg->data(), msg->size())) << std::endl;
    assert(msg->getMeta("md5").compare(md5(msg->data(), msg->size())) == 0);
    nrReceptions++;
    bytesRecvd += msg->size();
  }
};

int main(int argc, char** argv, char** envp) {
  Node* pubNode = new Node("foo");
  Publisher* pub = new Publisher("foo");
  pubNode->addPublisher(pub);
  
	Node* subNode = new Node("foo");
  Subscriber* sub = new Subscriber("foo", new TestReceiver());
  subNode->addSubscriber(sub);
  
  pub->waitForSubscribers(1);
  
  char* buffer = (char*)malloc(BUFFER_SIZE);
  memset(buffer, 40, BUFFER_SIZE);
  
  
  for (int i = 0; i < 10000; i++) {
    Thread::sleepMs(20);
    Message* msg = new Message(Message(buffer, BUFFER_SIZE));
    msg->setMeta("md5", md5(buffer, BUFFER_SIZE));
    pub->send(msg);
  }
  
  Thread::sleepMs(50);
  
  assert(nrReceptions == 100);
  assert(bytesRecvd == nrReceptions * BUFFER_SIZE);
  
	return EXIT_SUCCESS;
}

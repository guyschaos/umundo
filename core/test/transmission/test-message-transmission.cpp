#include "umundo/core.h"
#include <iostream>
#include <stdio.h>

#define BUFFER_SIZE 1024*1024

using namespace umundo;

static int nrReceptions = 0;
static int bytesRecvd = 0;


class TestReceiver : public Receiver {
	void receive(Message* msg) {
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
    pub->send(buffer, BUFFER_SIZE);
  }
  
  Thread::sleepMs(50);
  
  assert(nrReceptions == 100);
  assert(bytesRecvd == nrReceptions * BUFFER_SIZE);
  
	return EXIT_SUCCESS;
}

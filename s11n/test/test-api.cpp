//#include "um_message.pb.h"
#include "um_person.pb.h"
//#include "um_typed_message.pb.h"
#include "s11n/TypedPublisher.h"
#include "s11n/TypedSubscriber.h"
#include "common/Node.h"
#include "common/Message.h"

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

class TestTypedSubscriber : public TypedReceiver {
	void receive(void* obj, Message* msg) {
		std::cout << "received " << msg->getMeta("type") << " " << ((Person*)obj)->name() << std::endl;
	}
};

int main(int argc, char** argv) {
#ifdef __GNUC__
	signal(SIGSEGV, handler);   // install our handler
#endif
	
	Node* mainNode = new Node();
	TestTypedSubscriber* tts = new TestTypedSubscriber();
	TypedPublisher* tPub = new TypedPublisher("fooChannel");
	TypedSubscriber* tSub = new TypedSubscriber("fooChannel", tts);
  tSub->registerType("person", new Person());
  tPub->registerType("person", new Person());

	mainNode->addPublisher(tPub);
  mainNode->addSubscriber(tSub);
  
	Person* person = new Person();
	person->set_id(234525);
	person->set_name("Captain FooBar");
  while(true) {
    Thread::sleepMs(1000);
    tPub->sendObj("person", person);
  }
}
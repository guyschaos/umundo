//#include "um_message.pb.h"
#include "um_person.pb.h"
//#include "um_typed_message.pb.h"
#include "s11n/TypedPublisher.h"
#include "s11n/TypedSubscriber.h"
#include "common/Node.h"
#include "common/Message.h"

using namespace umundo;

class TestTypedSubscriber : public TypedReceiver {
	void receive(void* obj, Message* msg) {
		std::cout << "received " << msg->getMeta("type") << " " << ((Person*)obj)->name() << std::endl;
	}
};

int main(int argc, char** argv) {
	
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
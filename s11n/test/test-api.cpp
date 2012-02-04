//#include "um_message.pb.h"
#include "um_person.pb.h"
//#include "um_typed_message.pb.h"
#include "TypedPublisher.h"

using namespace umundo;

int main(int argc, char** argv) {
	TypedPublisher* tPub = new TypedPublisher("fooChannel");
//	tPub->registerType("string", new UMStringMessage());
	Person* person = new Person();
	person->set_id(234525);
	person->set_name("Captain FooBar");
	tPub->sendObj("string", person);
}
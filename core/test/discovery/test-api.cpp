#include "discovery/Discovery.h"
#include "connection/Publisher.h"
#include "connection/Subscriber.h"
#include "common/stdInc.h"

#include <iostream>

using namespace umundo;

class TestReceiver : public Receiver {
public:
	std::string _name;
	TestReceiver(std::string name) : _name(name) {};
	void receive(char* buffer, size_t length) {
//		std::string data(data, length);
		std::cout << _name << " received " << length << "bytes:" << std::endl << buffer << std::endl;
	}	
};

#define BUFFER_SIZE 1024

int main(int argc, char** argv) {
		TestReceiver* testRecv = new TestReceiver("recv1");
		Publisher* pubFoo = new Publisher("fooChannel");
		Subscriber* subFoo = new Subscriber("fooChannel", testRecv);
		
//	  signal(SIGINT, sig_int);
		char buffer[BUFFER_SIZE];
		for (int i = 0; i < BUFFER_SIZE; i++) {
			buffer[i] = (char)i%255;
		}
	
		while(1) {
      Thread::sleepMs(1000);
//			pubFoo->send(buffer, BUFFER_SIZE);
		}
}

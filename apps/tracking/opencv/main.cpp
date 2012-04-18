#include "ArucoPosePublisher.h"

using namespace umundo;

int main(int argc, char** argv) {
	ArucoPosePublisher* posePub = new ArucoPosePublisher("", "isight.yml");
	posePub->start();
	while(true) {
		Thread::sleepMs(500);
	}
}
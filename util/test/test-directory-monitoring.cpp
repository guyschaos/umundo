#include "umundo/core.h"
#include "umundo/util.h"
#include <iostream>
#include <stdio.h>

using namespace umundo;

int main(int argc, char** argv, char** envp) {
	if (argc != 2) {
		LOG_ERR("single argument is expected to be a directory");
		exit(EXIT_FAILURE);
	}
	Node* node = new Node();
	DirectoryMonitor* dirMon = new DirectoryMonitor(argv[1], "dirChanges");
	node->connect(dirMon);
	dirMon->start();
	std::cout << "Press CTRL+C to quit" << std::endl;
	while(true) {
		Thread::sleepMs(500);
	}
	delete dirMon;
	exit(EXIT_SUCCESS);

}

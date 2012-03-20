#include "umundo/core.h"
#include <iostream>
#include <stdio.h>

using namespace umundo;

static int receives = 0;
static Monitor monitor;

class TestDiscoverer : public ResultSet<NodeStub> {
public:
	TestDiscoverer() {
	}
	void added(shared_ptr<NodeStub> node) {
		printf("node added!\n");
		assert(node->getIP().size() >= 7);
		receives++;
		UMUNDO_SIGNAL(monitor);
	}
	void removed(shared_ptr<NodeStub> node) {

	}
	void changed(shared_ptr<NodeStub> node) {

	}
};

class TestDiscoverable : public Node {
public:
	TestDiscoverable(string domain) : Node(domain) {
	}
};

int main(int argc, char** argv, char** envp) {
	TestDiscoverer* testDiscoverer = new TestDiscoverer();
	shared_ptr<NodeQuery> query = shared_ptr<NodeQuery>(new NodeQuery("fooDomain", testDiscoverer));
	Discovery::browse(query);
	TestDiscoverable* testDiscoverable = new TestDiscoverable("fooDomain");
	Discovery::add(testDiscoverable);
	while(receives < 1)
		UMUNDO_WAIT(monitor);
}

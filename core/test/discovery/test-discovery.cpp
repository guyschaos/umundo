#include "discovery/Discovery.h"
#include "dns_sd.h"

#include <iostream>

using namespace umundo;

class TestNode : public Node, public ResultSet<NodeStub> {
public:
	TestNode() {
    _port = 3000;
    _transport = "tcp";
  }
	virtual void added(boost::shared_ptr<NodeStub> node) {
		std::cout << "Added: " << node.get() << std::endl;
	}
	virtual void removed(boost::shared_ptr<NodeStub> node) {
		std::cout << "Removed: " << node.get() << std::endl;
	}
	virtual void changed(boost::shared_ptr<NodeStub> node) {
		std::cout << "Changed: " << node.get() << std::endl;
	}
};

int main(int argc, char** argv) {
  boost::shared_ptr<TestNode> node = boost::shared_ptr<TestNode>(new TestNode());
  boost::shared_ptr<NodeQuery> query = boost::shared_ptr<NodeQuery>(new NodeQuery("", node.get()));
  Discovery::add(node);
  Discovery::browse(query.get());

//	Discovery::add(boost::shared_ptr<Node>(new Node(10001, "FOO1")));
	// NodeDiscoverer::add(boost::shared_ptr<Node>(new Node(10002, "FOO2")));
	// NodeDiscoverer::add(boost::shared_ptr<Node>(new Node(10003, "FOO3")));
	// NodeDiscoverer::add(boost::shared_ptr<Node>(new Node(10004, "FOO4", "local.")));
	// NodeDiscoverer::add(boost::shared_ptr<Node>(new Node(10005, "FOO5", "local.")));
	// NodeDiscoverer::add(boost::shared_ptr<Node>(new Node(10006, "FOO6", "local.")));
  getchar();
}
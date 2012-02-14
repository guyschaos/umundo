#include <umundo/core.h>
#include <umundo/s11n.h>


int main(int argc, char** argv) {
	
	Node node1 = new Node();

	ServiceManager svcMgr = new ServiceManager();
	
	EchoService echoSvc = new EchoService();
	PingService pingSvc = new PingService();

	echoSvc->advertiseOn(node1);
	pingSvc->advertiseOn(node1);
	
	
}
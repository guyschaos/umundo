#include <umundo/core.h>
#include <umundo/s11n.h>
#include <umundo/rpc.h>

#include "rpc/services/protobuf/EchoService.rpc.pb.h"
#include "rpc/services/protobuf/EchoService.pb.h"

using namespace umundo;

class EchoServiceImpl : public EchoService {
public:
	EchoReply* echo(EchoRequest* req) {
		EchoReply* reply = new EchoReply();
    reply->set_name(req->name());
		return reply;
	}
};

class PingServiceImpl : public PingService {
public:
	PingReply* ping(PingRequest* req) {
		PingReply* reply = new PingReply();
    reply->set_name("pong");
		return reply;
	}
};

int main(int argc, char** argv) {
	
	Node* node1 = new Node();
	ServiceManager* svcMgr1 = new ServiceManager(node1);
	svcMgr1->registerService(new EchoServiceImpl());
	svcMgr1->registerService(new PingServiceImpl());

  Node* node2 = new Node();
	ServiceManager* svcMgr2 = new ServiceManager(node2);

	PingServiceStub* pingSvc = new PingServiceStub(svcMgr2);
  PingRequest* pingReq = new PingRequest();
  pingReq->set_name("ping");
	PingReply* pingRep = pingSvc->ping(pingReq);
  std::cout << pingRep->name() << std::endl;
	delete pingReq;
	delete pingRep;
	
	int i = 100;
	EchoServiceStub* echoSvc = new EchoServiceStub(svcMgr2);
  EchoRequest* echoReq = new EchoRequest();
	while (i > 0) {
	  echoReq->set_name(".");
		EchoReply* echoRep = echoSvc->echo(echoReq);
	  std::cout << echoRep->name() << std::flush;
		delete echoRep;
	}
	delete echoReq;


}
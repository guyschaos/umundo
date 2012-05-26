#include <umundo/core.h>
#include <umundo/s11n.h>
#include <umundo/rpc.h>

#include "protobuf/generated/TestServices.rpc.pb.h"
#include "protobuf/generated/TestServices.pb.h"

using namespace umundo;

#define BUFFER_SIZE 1024 * 1024

class EchoServiceImpl : public EchoService {
public:
	EchoReply* echo(EchoRequest* req) {
		EchoReply* reply = new EchoReply();
		reply->set_name(req->name());
		reply->set_buffer(req->buffer().c_str(), BUFFER_SIZE);
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
  char* buffer = (char*)malloc(BUFFER_SIZE);
  for (int i = 0; i < BUFFER_SIZE; i++)
    buffer[i] = (char)i + 1;
  
  Node* node1 = new Node();
  Node* node2 = new Node();

  for (int i = 0; i < 10; i++) {
    EchoServiceImpl* localEchoService = new EchoServiceImpl();
    PingServiceImpl* localPingService = new PingServiceImpl();
    
    ServiceManager* svcMgr1 = new ServiceManager();
    node1->connect(svcMgr1);
    svcMgr1->addService(localEchoService);
    svcMgr1->addService(localPingService);

    ServiceManager* svcMgr2 = new ServiceManager();
    node2->connect(svcMgr2);

    ServiceFilter* pingSvcFilter = new ServiceFilter("PingService");
    PingServiceStub* pingSvc = new PingServiceStub(svcMgr2->find(pingSvcFilter));
    delete pingSvcFilter;
    
    PingRequest* pingReq = new PingRequest();
    pingReq->set_name("ping");
    PingReply* pingRep = pingSvc->ping(pingReq);
    std::cout << pingRep->name() << std::endl;
    delete pingReq;
    delete pingRep;

    // test rpc throughput with the echo service
    int iterations = 15;
    int sends = 0;

    ServiceFilter* echoSvcFilter = new ServiceFilter("EchoService");
    EchoServiceStub* echoSvc = new EchoServiceStub(svcMgr2->find(echoSvcFilter));
    time_t now;
    time_t start;
    time(&start);
    while (iterations > 0) {
      EchoRequest* echoReq = new EchoRequest();
      echoReq->set_name(".");
      echoReq->set_buffer(buffer, BUFFER_SIZE);

      EchoReply* echoRep = echoSvc->echo(echoReq);
      assert(echoRep->name().compare(".") == 0);
      const char* repBuffer = echoRep->buffer().c_str();
      assert(memcmp(repBuffer, buffer, BUFFER_SIZE) == 0);
      (void)repBuffer;
      
      sends++;
      time(&now);
      if ((now - start) > 0) {
        iterations--;
        std::cout << sends << " messages per second " << iterations << " iterations remaining" << std::endl;
        time(&start);
        sends = 0;
      }
      delete echoReq;
      delete echoRep;
    }
    
    svcMgr1->removeService(localEchoService);
    svcMgr1->removeService(localPingService);
    delete localEchoService;
    delete localPingService;

    node1->disconnect(svcMgr1);
    node2->disconnect(svcMgr2);
    delete svcMgr1;
    delete svcMgr2;
  }
}
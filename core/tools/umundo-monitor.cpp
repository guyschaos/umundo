#include "umundo/config.h"
#include "umundo/core.h"
#include "umundo/core.h"
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include "XGetopt.h"
#endif

#ifdef DISC_BONJOUR
#include "umundo/discovery/bonjour/BonjourNodeDiscovery.h"
#define DISC_IMPL BonjourNodeDiscovery
#endif
#ifdef DISC_AVAHI
#include "umundo/discovery/avahi/AvahiNodeDiscovery.h"
#define DISC_IMPL AvahiNodeDiscovery
#endif
#if !(defined DISC_AVAHI || defined DISC_BONJOUR)
#error "No discovery implementation choosen"
#endif

#ifdef NET_ZEROMQ
#include "umundo/connection/zeromq/ZeroMQNode.h"
#include "umundo/connection/zeromq/ZeroMQPublisher.h"
#include "umundo/connection/zeromq/ZeroMQSubscriber.h"
#define NET_NODE_IMPL ZeroMQNode
#define NET_PUB_IMPL ZeroMQPublisher
#define NET_SUB_IMPL ZeroMQSubscriber
#endif
#if !(defined NET_ZEROMQ)
#error "No discovery implementation choosen"
#endif

using namespace umundo;

char* channel = NULL;
char* domain = NULL;
char* file = NULL;
bool interactive = false;
bool verbose = false;
int minSubs = 0;

class DiscoveryMonitor : public DISC_IMPL {};
class NodeMonitor : public NET_NODE_IMPL {};
class PublisherMonitor : public NET_PUB_IMPL {};
class SubscriberMonitor : public NET_SUB_IMPL {};

void printUsageAndExit() {
	printf("umundo-monitor version 0.0.2\n");
	printf("Usage\n");
	printf("\tumundo-monitor -c channel [-iv] [-d domain] [-f file]\n");
	printf("\n");
	printf("Options\n");
	printf("\t-c <channel>       : use channel\n");
	printf("\t-d <domain>        : join domain\n");
	printf("\t-f <file>          : publish contents of file\n");
	printf("\t-w <number>        : wait for given number of subscribers before publishing\n");	
	printf("\t-i                 : interactive mode (simple chat)\n");	
	printf("\t-v                 : be more verbose\n");	
	exit(1);
}

class PlainDumpingReceiver : public Receiver {
  void receive(Message* msg) {
    std::cout << "Received!" << msg->getData() << std::flush;
  }
};

int main(int argc, char** argv) {
	// Factory::registerPrototype("discovery", new DiscoveryMonitor(), NULL);
	// Factory::registerPrototype("node", new NodeMonitor(), NULL);
	// Factory::registerPrototype("publisher", new PublisherMonitor(), NULL);
	// Factory::registerPrototype("subscriber", new SubscriberMonitor(), NULL);

	int option;
	while ((option = getopt(argc, argv, "ivd:f:c:w:")) != -1) {
		switch(option) {
			case 'c':
				channel = optarg;
				break;
			case 'd':
				domain = optarg;
				break;
			case 'f':
				file = optarg;
				break;
			case 'w':
				minSubs = atoi(optarg);
				break;
			case 'i':
				interactive = true;
				break;
			case 'v':
				interactive = true;
				break;
			default:
				printUsageAndExit();
		}
	}
	
	if (!channel)
		printUsageAndExit();
	
	Node* node = NULL;
	Publisher* pub = NULL;
	Subscriber* sub = NULL;
  
	if (domain) {
		node = new Node(domain);
	} else {
		node = new Node();
	}
	
	/**
	 * Send file content
	 */
	if (file) {
    if (pub == NULL) {
      pub = new Publisher(channel);
      node->addPublisher(pub);
      pub->waitForSubscribers(minSubs);
    }

		FILE *fp;
		fp = fopen(file, "r");
		if (fp == NULL) {
			printf("Failed to open file %s: %s\n", file, strerror(errno));
			return EXIT_FAILURE;
		}
		
		int read = 0;
		int lastread = 0;
		char* readBuffer = (char*) malloc(1000);
		while(true) {
			lastread = fread(readBuffer, 1, 1000, fp);

			if(ferror(fp)) {
				printf("Failed to read from file %s: %s", file, strerror(errno));
				return EXIT_FAILURE;
			}

			if (lastread <= 0)
				break;
			
			pub->send(readBuffer, lastread);
			read += lastread;
      
      if (feof(fp))
        break;
		}
		fclose(fp);
		printf("Send %d bytes on channel \"%s\"\n", read, channel);
    if (!interactive)
      exit(0);
	}

  if (sub == NULL) {
    sub = new Subscriber(channel, new PlainDumpingReceiver());
    node->addSubscriber(sub);
  }

  if (interactive) {
    /**
     * Enter interactive mode
     */
    if (pub == NULL) {
      pub = new Publisher(channel);
      node->addPublisher(pub);
    }
    pub->waitForSubscribers(minSubs);
    string line;
    while(std::cin) {
      getline(std::cin, line);
      line.append("\n");
      pub->send(line.c_str(), line.length());
    };
  } else {
    /**
     * Non-interactive, just let the subscriber print channel messages
     */
    while (true)
      Thread::sleepMs(500);
  }
  
}


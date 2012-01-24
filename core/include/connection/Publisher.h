#ifndef PUBLISHER_H_F3M1RWLN
#define PUBLISHER_H_F3M1RWLN

#include "common/stdInc.h"
#include "common/NodeStub.h"

namespace umundo {

class PublisherImpl {
public:
  PublisherImpl() {}
  virtual ~PublisherImpl() {}
  PublisherImpl(string channelname) : _channelName(channelname) {}
	virtual PublisherImpl* create(string channelName) = 0;
	
	virtual void send(char* buffer, size_t length) = 0;

	virtual const string& getChannelName() { return _channelName; }

	string _channelName;
};

class PublisherStub : public EndPoint {
public:
	PublisherStub(string channelName) : _channelName(channelName) {};
	virtual ~PublisherStub() {}

  virtual shared_ptr<NodeStub> getNode() { return _node; }
  virtual void setNode(shared_ptr<NodeStub> node) { _node = node; }
	virtual const string& getChannelName() { return _channelName; }
	
protected:
	string _channelName;
  shared_ptr<NodeStub> _node;
};

class Publisher : public PublisherStub {
public:
	Publisher(const string& channelName);
	virtual ~Publisher();
	
	const string& getChannelName() { return _impl->getChannelName(); }
	void send(char* buffer, size_t length);

protected:
	PublisherImpl* _impl;

};

}


#endif /* end of include guard: PUBLISHER_H_F3M1RWLN */

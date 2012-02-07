#ifndef PUBLISHER_H_F3M1RWLN
#define PUBLISHER_H_F3M1RWLN

#include "umundo/common/Common.h"
#include "umundo/common/EndPoint.h"
#include "umundo/common/Implementation.h"

namespace umundo {

class NodeStub;
class Message;	

class PublisherConfig : public Configuration {
public:
	shared_ptr<Configuration> create();
	
	string channelName;
	string transport;
	uint16_t port;
};

/**
 * Representation of a remote Publisher.
 */
class PublisherStub : public EndPoint {
public:
	PublisherStub() {}
	virtual ~PublisherStub() {}

	/** @name Functionality of local *and* remote Publishers */
  //@{
	virtual const string& getChannelName() const      { return _channelName; }
	virtual void setChannelName(string channelName)   { _channelName = channelName; }
	virtual shared_ptr<NodeStub> getNode() const      { return _node; }
	virtual void setNode(shared_ptr<NodeStub> node)   { _node = node; }
	virtual const string& getHost() const             { return _host; }
	virtual void setHost(string host)                 { _host = host; }
	virtual const string& getDomain() const           { return _domain; }
	virtual void setDomain(string domain)             { domain = _domain; }
	//@}
	
protected:
	string _channelName;
	shared_ptr<NodeStub> _node;
	string _host;
	string _domain;
};

/**
 * Publisher implementor basis class (bridge pattern)
 */
class PublisherImpl : public Implementation, public PublisherStub {
public:
	PublisherImpl() {}
	virtual ~PublisherImpl() {}

	virtual void send(Message* msg) = 0;

};

/**
 * Abstraction for publishing byte-arrays on channels (bridge pattern).
 *
 * We need to overwrite everything and use the concrete implementors fields.
 */
class Publisher : private PublisherStub {
public:
	Publisher(const string& channelName);
	virtual ~Publisher();

	/** @name Functionality of local Publishers */
  //@{
	void send(Message* msg)       { _impl->send(msg); }
	void send(const char* data, size_t length);
	//@}

	/** @name Overwrite PublisherStub */
  //@{
	virtual const string& getChannelName() const      { return _impl->getChannelName(); }
	virtual void setChannelName(string channelName)   { _impl->setChannelName(channelName); }
	virtual shared_ptr<NodeStub> getNode() const      { return _impl->getNode(); }
	virtual void setNode(shared_ptr<NodeStub> node)   { _impl->setNode(node); }
	//@}

	/** @name Overwrite EndPoint */
	virtual const string& getIP() const         { return _impl->getIP(); }
	virtual void setIP(string ip)               { _impl->setIP(ip); }
	virtual const string& getTransport() const  { return _impl->getTransport(); }
	virtual void setTransport(string transport) { _impl->setTransport(transport); }
	virtual uint16_t getPort() const            { return _impl->getPort(); }
	virtual void setPort(uint16_t port)         { _impl->setPort(port); }
	virtual bool isRemote() const               { return _impl->isRemote(); }
	virtual void setRemote(bool remote)         { _impl->setRemote(remote); }
	virtual const string& getHost() const       { return _impl->getHost(); }
	virtual void setHost(string host)           { _impl->setHost(host); }
	virtual const string& getDomain() const     { return _impl->getDomain(); }
	virtual void setDomain(string domain)       { _impl->setDomain(domain); }
	//@{

protected:
	shared_ptr<PublisherImpl> _impl;
	shared_ptr<PublisherConfig> _config;
	friend class Node;
};

}


#endif /* end of include guard: PUBLISHER_H_F3M1RWLN */

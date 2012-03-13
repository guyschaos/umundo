#ifndef NODE_H_AA94X8L6
#define NODE_H_AA94X8L6

#include "umundo/common/Common.h"
#include "umundo/common/EndPoint.h"
#include "umundo/common/Implementation.h"

#define SHORT_UUID(uuid) uuid.substr(0, 8)

namespace umundo {

class Publisher;
class PublisherImpl;
class Subscriber;
class SubscriberImpl;
class NodeStub;

/**
 * Representation of a remote umundo Node.
 */
class NodeStub : public EndPoint {
public:
	virtual ~NodeStub() {}

	/** @name Remote Node */
	//@{
	virtual const string& getUUID() const       {
		return _uuid;
	}
	virtual void setUUID(string uuid)           {
		_uuid = uuid;
	}
	//@}

	inline bool operator==(NodeStub* n) const {
		return (getUUID().compare(n->getUUID()) == 0);
	}

	inline bool operator!=(NodeStub* n) const {
		return (getUUID().compare(n->getUUID()) != 0);
	}

protected:
	string _uuid;

private:
	friend std::ostream& operator<<(std::ostream&, const NodeStub*);

};

class NodeConfig : public Configuration {
public:
	shared_ptr<Configuration> create();
	virtual ~NodeConfig() {}

	string domain;
	string transport;
	string host;
	uint16_t port;
	string uuid;
};

class Node;
/**
 * The local umundo node implementor basis class (bridge pattern).
 */
class NodeImpl : public Implementation, public NodeStub {
public:
	NodeImpl();
	virtual ~NodeImpl() {}

	/** @name Publish / Subscriber Maintenance */
	//@{
	virtual void addSubscriber(shared_ptr<SubscriberImpl>) = 0;
	virtual void removeSubscriber(shared_ptr<SubscriberImpl>) = 0;
	virtual void addPublisher(shared_ptr<PublisherImpl>) = 0;
	virtual void removePublisher(shared_ptr<PublisherImpl>) = 0;
	//@}
};

/**
 * The local umundo node abstraction (bridge pattern).
 */
class Node : public NodeStub {
public:
	Node();
	Node(string domain);
	virtual ~Node();

	/** @name Publish / Subscriber Maintenance */
	//@{
	void addSubscriber(Subscriber*);
	void removeSubscriber(Subscriber*);
	void addPublisher(Publisher*);
	void removePublisher(Publisher*);
	//@}

	/** @name EndPoint */
	virtual const string& getIP() const         {
		return _impl->getIP();
	}
	virtual void setIP(string ip)               {
		_impl->setIP(ip);
	}
	virtual const string& getTransport() const  {
		return _impl->getTransport();
	}
	virtual void setTransport(string transport) {
		_impl->setTransport(transport);
	}
	virtual uint16_t getPort() const            {
		return _impl->getPort();
	}
	virtual void setPort(uint16_t port)         {
		_impl->setPort(port);
	}
	virtual bool isRemote() const               {
		return _impl->isRemote();
	}
	virtual void setRemote(bool remote)         {
		_impl->setRemote(remote);
	}
	//@{

	/** @name Node Stub */
	virtual const string& getUUID() const       {
		return _impl->getUUID();
	}
	virtual void setUUID(string uuid)           {
		_impl->setUUID(uuid);
	}
	virtual const string& getHost() const       {
		return _impl->getHost();
	}
	virtual void setHost(string host)           {
		_impl->setHost(host);
	}
	virtual const string& getDomain() const     {
		return _impl->getDomain();
	}
	virtual void setDomain(string domain)       {
		_impl->setDomain(domain);
	}
	//@{

  static int instances;

protected:
	shared_ptr<NodeImpl> _impl;
	shared_ptr<NodeConfig> _config;

	friend class Discovery;
};

}


#endif /* end of include guard: NODE_H_AA94X8L6 */

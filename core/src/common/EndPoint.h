#ifndef ENDPOINT_H_2O8TQUBP
#define ENDPOINT_H_2O8TQUBP

#include "common/Common.h"

namespace umundo {
/**
 * Anything that is addressable in TCP/IP networks.
 */
class EndPoint {
public:
	virtual const string& getIP() const           { return _ip; }
	virtual void setIP(string ip)                 { _ip = ip; }
	virtual const string& getTransport() const    { return _transport; }
	virtual void setTransport(string transport)   { _transport = transport; }
	virtual uint16_t getPort() const              { return _port; }
	virtual void setPort(uint16_t port)           { _port = port; }
	virtual bool isRemote() const                 { return _isRemote; }
	virtual void setRemote(bool isRemote)         { _isRemote = isRemote; }
	virtual const string& getHost() const         { return _host; }
	virtual void setHost(string host)             { _host = host; }
	virtual const string& getDomain() const       { return _domain; }
	virtual void setDomain(string domain)         { _domain = domain; }

protected:
	string _ip;
	string _transport;
	uint16_t _port;
	bool _isRemote;
	string _host;
	string _domain;

};
}


#endif /* end of include guard: ENDPOINT_H_2O8TQUBP */

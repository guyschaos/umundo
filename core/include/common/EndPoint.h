#ifndef ENDPOINT_H_2O8TQUBP
#define ENDPOINT_H_2O8TQUBP

#include "common/stdInc.h"

namespace umundo {
/**
 * An Endpoint is anything that has an address.
 *
 * At the moment this is only TCP/UDP on top of IP.
 */
class EndPoint {
public:
	virtual const string& getIP();
	virtual void setIP(string);

	virtual const string& getHost();
	virtual void setHost(string);

	virtual const string& getTransport();
	virtual void setTransport(string);

	virtual const string& getDomain();
	virtual void setDomain(string);

	virtual uint16_t getPort();
	virtual void setPort(uint16_t);

protected:
	string _host;
	string _transport;
	string _domain;
	string _ip;
	uint16_t _port;
};
}


#endif /* end of include guard: ENDPOINT_H_2O8TQUBP */

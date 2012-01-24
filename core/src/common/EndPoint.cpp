#include "common/EndPoint.h"

namespace umundo {

const string& EndPoint::getIP() {
	return _ip;
}
void EndPoint::setIP(string ip) {
	_ip = ip;
}

const string& EndPoint::getHost() {
	return _host;
}
void EndPoint::setHost(string host) {
	_host = host;
}

const string& EndPoint::getTransport() {
	return _transport;
}
void EndPoint::setTransport(string transport) {
	_transport = transport;
}

const string& EndPoint::getDomain() {
	return _domain;
}
void EndPoint::setDomain(string domain) {
	_domain = domain;
}

uint16_t EndPoint::getPort() {
	return _port;
}
void EndPoint::setPort(uint16_t port) {
	_port = port;
}
}
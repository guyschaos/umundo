#include "common/Message.h"

namespace umundo {

Message::Message() {
}

char* Message::getData() {
	return _impl->getData();
}

void Message::setData(char* data) {
	_impl->setData(data);
}

int Message::getSize() {
	return _impl->getSize();
}

void Message::setSize(int size) {
	_impl->setSize(size);
}

char* Message::getRaw() {
	return _impl->getRaw();
}

void Message::setRaw(char* raw) {
	_impl->setRaw(raw);
}

int Message::getInt(std::string key) {
	return _impl->getInt(key);
}

void Message::setInt(std::string key, int value) {
	_impl->setInt(key, value);
}

std::string Message::getString(std::string key) {
	return _impl->getString(key);
}

void Message::setString(std::string key, std::string value) {
	_impl->setString(key, value);
}

}
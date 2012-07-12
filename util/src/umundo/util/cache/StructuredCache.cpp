/**
 *  Copyright (C) 2012  Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 */

#include "umundo/util/cache/StructuredCache.h"

#include <limits>

#ifdef WIN32
// max gets defined in windows.h and overrides numeric_limits<>::max
#undef max
#endif

namespace umundo {

SCache::SCache() {}

SCache::~SCache() {}

void SCache::pressure(float pressure) {
	_mutex.lock();
	_pressure = pressure;
	_mutex.unlock();
}

void SCache::run() {
	while(isStarted()) {
		// sleep here to avoid holding the lock all the time
		Thread::sleepMs(300);
		ScopeLock lock(&_mutex);
		std::cout << "Cache pruning!" << std::endl;

		map<intptr_t, SCacheItem*>::iterator itemIter = _cacheItems.begin();
		while(itemIter != _cacheItems.end()) {
			float pressure = 0;
			if (itemIter->second->_cache)
				pressure = itemIter->second->_cache->_pressure;
			int distance = itemIter->second->_distance;

			itemIter->second->alleviate(pressure, distance);
			itemIter++;
		}
	}
}

void SCache::resetDistances() {
	ScopeLock lock(&_mutex);

	// set distance to infinity
	map<intptr_t, SCacheItem*>::iterator itemIter = _cacheItems.begin();
	while(itemIter != _cacheItems.end()) {
		itemIter->second->_distance = std::numeric_limits<int>::max();
		itemIter++;
	}

	// calculate distance from all pointers
	set<weak_ptr<SCachePointer> >::iterator ptrIter = _cachePointers.begin();
	while(ptrIter != _cachePointers.end()) {
		shared_ptr<SCachePointer> ptr = ptrIter->lock();
		if (ptr.get() != NULL) {
			// no pressure on items pointed to
			ptr->getItem()->propagateDistance(0);
		} else {
			// weak pointer points to deleted object
			_cachePointers.erase(ptrIter);
		}
		ptrIter++;
	}
}

void SCache::insert(SCacheItem* item) {
	item->_cache = this;
	_cacheItems[(intptr_t)item] = item;
}

void SCache::remove(SCacheItem* item) {
	if(_cacheItems.find((intptr_t)item) != _cacheItems.end()) {
		item->_cache = NULL;
		_cacheItems.erase((intptr_t)item);
	}
}

bool SCache::isEmpty() {
	return (_cacheItems.size() == 0);
}

SCacheItem::~SCacheItem() {
}

void SCacheItem::propagateDistance(int distance) {
	if (_distance > distance) {
		_distance = distance;
	} else {
		return;
	}

	set<SCacheItem*> next = getNext();
	set<SCacheItem*>::iterator outIter = next.begin();
	while(outIter != next.end()) {
		if ((*outIter) != NULL)
			(*outIter)->propagateDistance(_distance + 1);
		outIter++;
	}
	return;
}

}
#include "umundo/util/cache/NBandCache.h"

using namespace umundo;

NBandProxyCacheItem::NBandProxyCacheItem() {
	_otherBand = NULL;
	_currentItem = NULL;
}

set<SCacheItem*> NBandProxyCacheItem::getNext() {
	set<SCacheItem*> next;
	next.insert((SCacheItem*)_otherBand);
	next.insert((SCacheItem*)_currentItem);
	return next;
}

void NBandProxyCacheItem::propagateDistance(int distance) {
	SCacheItem::propagateDistance(distance - 1);
}

NBandCacheItem::NBandCacheItem(string name, string band) : _name(name), _band(band), _isPrepared(false) {
	_left = NULL;
	_right = NULL;
	_up = NULL;
	_down = NULL;
}

set<SCacheItem*> NBandCacheItem::getNext() {
	set<SCacheItem*> next;
	next.insert(_left);
	next.insert(_right);
	next.insert(_up);
	next.insert(_down);
	return next;
}

NBandCachePtr::NBandCachePtr() {
	_item = NULL;
}

NBandCachePtr::~NBandCachePtr() {}

SCacheItem* NBandCachePtr::getItem() {
	return _item;
}

NBandCacheItem* NBandCachePtr::left(bool moveBand) {
	ScopeLock lock(&_cache->_mutex);
	if (_item->_left == NULL) {
		return NULL;
	}
	if (moveBand) {
		_item->_up->_currentItem = _item->_left;
		_item->_down->_currentItem = _item->_left;
	}
	_item = _item->_left;
	assert(_item != NULL);
	return _item;
}

NBandCacheItem* NBandCachePtr::right(bool moveBand) {
	ScopeLock lock(&_cache->_mutex);
	if (_item->_right == NULL) {
		return NULL;
	}
	if (moveBand) {
		_item->_up->_currentItem = _item->_right;
		_item->_down->_currentItem = _item->_right;
	}
	_item = _item->_right;
	assert(_item != NULL);
	return _item;
}

NBandCacheItem* NBandCachePtr::up() {
	ScopeLock lock(&_cache->_mutex);
	if (_item->_up->_otherBand == NULL) {
		return NULL;
	}
	_item = _item->_up->_otherBand->_currentItem;
	assert(_item != NULL);
	return _item;
}

NBandCacheItem* NBandCachePtr::down() {
	ScopeLock lock(&_cache->_mutex);
	if (_item->_down->_otherBand == NULL) {
		return NULL;
	}
	_item = _item->_down->_otherBand->_currentItem;
	assert(_item != NULL);
	return _item;
}

NBandCache::NBandCache() {}

shared_ptr<NBandCachePtr> NBandCache::getPointer() {
	ScopeLock lock(&_mutex);
	if (_bands.size() > 0) {
		return getPointer(_bands.begin()->first);
	} else {
		return getPointer("");
	}
}

shared_ptr<NBandCachePtr> NBandCache::getPointer(const string& band, int elemId) {
	ScopeLock lock(&_mutex);
	// is there such a band?
	BandIter bandIter = _bands.find(band);
	if (bandIter == _bands.end())
		return shared_ptr<NBandCachePtr>();

	// get the current item of the band
	shared_ptr<NBandCachePtr> ptr = shared_ptr<NBandCachePtr>(new NBandCachePtr());
	NBandCacheItem* item;
	if (elemId == -1) {
		item = bandIter->second.begin()->second->_up->_currentItem;
		ptr->_item = item;
	} else {
		item = bandIter->second.begin()->second;
		while(elemId-- > 0) {
			item = item->_right;
		}
		ptr->_item = item;
	}
	assert(ptr->_item != NULL);
	ptr->_cache = this;
	_cachePointers.insert(ptr);
	return ptr;
}

void NBandCache::insert(NBandCacheItem* item) {
	ScopeLock lock(&_mutex);
	SCache::insert((SCacheItem*)item);
	Proxies currProxies;
	// if there is no such band yet establish its proxy
	if (_proxies.find(item->_band) == _proxies.end()) {
		currProxies = std::make_pair(new NBandProxyCacheItem(), new NBandProxyCacheItem());
		SCache::insert((SCacheItem*)currProxies.first);
		SCache::insert((SCacheItem*)currProxies.second);
		_proxies[item->_band] = currProxies;
		currProxies.first->_currentItem = item;
		currProxies.second->_currentItem = item;
		// insert the band and connect with neighboring proxies
		ProxyIter proxyIter = _proxies.find(item->_band);

		if (proxyIter != _proxies.begin()) {
			// there is a proxy for the band above
			proxyIter--;
			proxyIter->second.second->_otherBand = currProxies.first;
			currProxies.first->_otherBand = proxyIter->second.second;
			proxyIter++;
		}

		if (++proxyIter != _proxies.end()) {
			// there is a proxy for the band below
			proxyIter->second.first->_otherBand = currProxies.second;
			currProxies.second->_otherBand = proxyIter->second.first;
		}
	} else {
		currProxies = _proxies[item->_band];
	}

	item->_up = currProxies.first;
	item->_down = currProxies.second;

	// insert the new item within the band
	if (_bands[item->_band].find(item->_name) == _bands[item->_band].end()) {
		_bands[item->_band][item->_name] = item;
		map<string, NBandCacheItem*>::iterator itemIter = _bands[item->_band].find(item->_name);
		if (itemIter != _bands[item->_band].begin()) {
			// there is an item before us in the band
			itemIter--;
			item->_left = itemIter->second;
			itemIter->second->_right = item;
			itemIter++;
		}
		if (++itemIter != _bands[item->_band].end()) {
			// there is an item after us in the band
			item->_right = itemIter->second;
			itemIter->second->_left = item;
		}
	}
}

void NBandCache::remove(NBandCacheItem* item) {
}

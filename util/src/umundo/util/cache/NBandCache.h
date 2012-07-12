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

#ifndef NBANDCACHE_H_MJROKJNY
#define NBANDCACHE_H_MJROKJNY

#include "umundo/util/cache/StructuredCache.h"

namespace umundo {

class NBandCacheItem;

class DLLEXPORT NBandProxyCacheItem : public SCacheItem {
public:
	NBandProxyCacheItem();
	set<SCacheItem*> getNext();

	void alleviate(float pressure, int distance) {} // do nothing
	void propagateDistance(int distance);

	NBandProxyCacheItem* _otherBand;
	NBandCacheItem* _currentItem;
};


class DLLEXPORT NBandCacheItem : public SCacheItem {
public:
	NBandCacheItem(string name, string band);

	set<SCacheItem*> getNext();
	void alleviate(float pressure, int distance) = 0;

	NBandCacheItem* _left;
	NBandCacheItem* _right;
	NBandProxyCacheItem* _up;
	NBandProxyCacheItem* _down;

	string _name;
	string _band;
	bool _isPrepared;
};

class DLLEXPORT NBandCachePtr : public SCachePointer {
public:
	NBandCachePtr();

	virtual ~NBandCachePtr();
	SCacheItem* getItem();

	NBandCacheItem* left(bool moveBand = false);
	NBandCacheItem* right(bool moveBand = false);
	NBandCacheItem* up();
	NBandCacheItem* down();

	const string getItemName();
	const string getBandName();
	const int getElementId();

	NBandCacheItem* _item;
};

class DLLEXPORT NBandCache : public SCache {
public:
	NBandCache();

	virtual shared_ptr<NBandCachePtr> getPointer();
	virtual shared_ptr<NBandCachePtr> getPointer(const string& band, int elemId = -1);
	virtual void insert(NBandCacheItem* item);
	virtual void remove(NBandCacheItem* item);

protected:
	typedef map<string, map<string, NBandCacheItem*> >::iterator BandIter;
	typedef map<string, std::pair<NBandProxyCacheItem*, NBandProxyCacheItem*> >::iterator ProxyIter;
	typedef std::pair<NBandProxyCacheItem*, NBandProxyCacheItem*> Proxies;

	map<string, map<string, NBandCacheItem*> > _bands; // band to names to items
	map<string, std::pair<NBandProxyCacheItem*, NBandProxyCacheItem*> > _proxies;  // band to its proxy (up, down)
};

}

#endif /* end of include guard: NBANDCACHE_H_MJROKJNY */

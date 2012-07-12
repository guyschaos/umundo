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

#ifndef SCACHE_H_LAY4Q1LN
#define SCACHE_H_LAY4Q1LN

#include <umundo/core.h>

namespace umundo {

class SCachePointer;
class SCache;
class SCacheItem;

/**
 * A SCachePointer is the sole handle for applications to access items in the structured cache.
 *
 * It is important to exclusively use SCachePointers or subclasses thereof retrieved from the cache or
 * subclasses thereof to access SCacheItems in the cache as this will allow the cache to page out items
 * that are "father away" form the pointers.
 */
class DLLEXPORT SCachePointer {
public:

	SCachePointer() {}
	virtual SCacheItem* getItem() = 0;

	SCache* _cache;
	friend class SCache;
};

/**
 * A node in the structure of the cache with the actual data.
 *
 * Subclasses of this class are expected to alleviate the pressure put on them by
 * paging out their contents. If the pressure drops, they are to reload their content.
 */
class DLLEXPORT SCacheItem {
public:
	virtual ~SCacheItem();

	virtual void alleviate(float pressure, int distance) = 0;
	virtual void propagateDistance(int distance);

	virtual set<SCacheItem*> getNext() = 0;

protected:
	int _distance;
	SCache* _cache;

	friend class SCache;
	friend class SCachePointer;
};

/**
 * A SCache orders its items in a graph and puts pressure on those far away from items pointed to.
 */
class DLLEXPORT SCache : public Thread {
public:
	SCache();
	virtual ~SCache();

	void insert(SCacheItem*);
	void remove(SCacheItem*);
	virtual bool isEmpty();

	void run();
	void pressure(float pressure);
	void alleviate();
	void resetDistances();

	Mutex _mutex;
	float _pressure;
	map<intptr_t, SCacheItem*> _cacheItems;
	set<weak_ptr<SCachePointer> > _cachePointers;

	friend class SCachePointer;
	friend class SCacheItem;
};

}
#endif /* end of include guard: SCACHE_H_LAY4Q1LN */

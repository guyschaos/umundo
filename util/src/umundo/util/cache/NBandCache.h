#ifndef NBANDCACHE_H_MJROKJNY
#define NBANDCACHE_H_MJROKJNY

#include "umundo/util/cache/StructuredCache.h"

namespace umundo {

class NBandCacheItem;

class NBandProxyCacheItem : public SCacheItem {
public:
	NBandProxyCacheItem();
	set<SCacheItem*> getNext();

	void alleviate(float pressure, int distance) {} // do nothing
	void propagateDistance(int distance);

	NBandProxyCacheItem* _otherBand;
	NBandCacheItem* _currentItem;
};


class NBandCacheItem : public SCacheItem {
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

class NBandCachePtr : public SCachePointer {
public:
	NBandCachePtr();

	virtual ~NBandCachePtr();
	SCacheItem* getItem();

	NBandCacheItem* left(bool moveBand = false);
	NBandCacheItem* right(bool moveBand = false);
	NBandCacheItem* up();
	NBandCacheItem* down();

	NBandCacheItem* _item;
};

class NBandCache : public SCache {
public:
	NBandCache();

	shared_ptr<NBandCachePtr> getPointer();
	shared_ptr<NBandCachePtr> getPointer(const string& band, int elemId = -1);
	void insert(NBandCacheItem* item);
	void remove(NBandCacheItem* item);

protected:
	typedef map<string, map<string, NBandCacheItem*> >::iterator BandIter;
	typedef map<string, std::pair<NBandProxyCacheItem*, NBandProxyCacheItem*> >::iterator ProxyIter;
	typedef std::pair<NBandProxyCacheItem*, NBandProxyCacheItem*> Proxies;

	map<string, map<string, NBandCacheItem*> > _bands; // band to names to items
	map<string, std::pair<NBandProxyCacheItem*, NBandProxyCacheItem*> > _proxies;  // band to its proxy (up, down)
};

}

#endif /* end of include guard: NBANDCACHE_H_MJROKJNY */

#include "umundo/core.h"
#include "umundo/util.h"
#include <iostream>
#include <stdio.h>

using namespace umundo;

class StringCacheItem : public NBandCacheItem {
public:
	StringCacheItem(string data, string name, string band) : NBandCacheItem(name, band), _data(data) {}
	void alleviate(float pressure, int distance) {
	}

	string _data;
};

int main(int argc, char** argv, char** envp) {
	NBandCache* nbCache = new NBandCache();
	nbCache->insert(new StringCacheItem("element ", "00", "0"));
	nbCache->insert(new StringCacheItem("element ", "01", "0"));
	nbCache->insert(new StringCacheItem("element ", "02", "0"));
	nbCache->insert(new StringCacheItem("element ", "00", "1"));
	nbCache->insert(new StringCacheItem("element ", "01", "1"));
	nbCache->insert(new StringCacheItem("element ", "02", "1"));
	nbCache->insert(new StringCacheItem("element ", "00", "2"));
	nbCache->start();
	{
		shared_ptr<NBandCachePtr> nbPtr = nbCache->getPointer("2");
		assert(nbPtr.get() != NULL);
		assert(nbPtr->left() == NULL);
		Thread::sleepMs(500);

		assert(nbPtr->right() == NULL);
		Thread::sleepMs(500);

		assert(nbPtr->down() == NULL);
		Thread::sleepMs(500);

		NBandCacheItem* l1 = nbPtr->up();
		(void)l1;
		assert(l1 != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->right() != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->right() != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->right() == NULL);
		Thread::sleepMs(500);

		assert(nbPtr->up() != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->down() != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->_item == l1);
		assert(nbPtr->right(true) != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->up() != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->down() != NULL);
		Thread::sleepMs(500);

		assert(nbPtr->left() == l1);
		Thread::sleepMs(500);

		//	nbCache->applyPressure(1.0);
	}
}








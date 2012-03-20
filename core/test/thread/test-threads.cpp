#include "umundo/core.h"
#include <iostream>

using namespace umundo;

bool testRecursiveMutex() {
	Mutex mutex;
	UMUNDO_LOCK(mutex);
	if(!mutex.tryLock()) {
		LOG_ERR("tryLock should be possible from within the same thread");
		assert(false);
	}
	UMUNDO_LOCK(mutex);
	return true;
}

static Mutex testMutex;
bool testThreads() {
	class Thread1 : public Thread {
		void run() {
			if(testMutex.tryLock()) {
				LOG_ERR("tryLock should return false with a mutex locked in another thread");
				assert(false);
			}
			UMUNDO_LOCK(testMutex); // blocks
			Thread::sleepMs(50);
			UMUNDO_UNLOCK(testMutex);
			Thread::sleepMs(100);
		}
	};

	UMUNDO_LOCK(testMutex);
	Thread1 thread1;
	thread1.start();
	Thread::sleepMs(50); // thread1 will trylock and block on lock
	UMUNDO_UNLOCK(testMutex);  // unlock
	Thread::sleepMs(20); // yield cpu and sleep
	// thread1 sleeps with lock on mutex
	if(testMutex.tryLock()) {
		LOG_ERR("tryLock should return false with a mutex locked in another thread");
		assert(false);
	}
	UMUNDO_LOCK(testMutex);    // thread1 will unlock and sleep
	thread1.join();      // join with thread1
	if(thread1.isStarted()) {
		LOG_ERR("thread still running after join");
		assert(false);
	}

	return true;
}

static Monitor testMonitor;
static int passedMonitor = 0;
bool testMonitors() {
	class Thread1 : public Thread {
		void run() {
			UMUNDO_WAIT(testMonitor);
			Thread::sleepMs(10); // avoid clash with other threads
			passedMonitor++;
		}
	};
	class Thread2 : public Thread {
		void run() {
			UMUNDO_WAIT(testMonitor);
			Thread::sleepMs(5);
			passedMonitor++;
		}
	};
	class Thread3 : public Thread {
		void run() {
			UMUNDO_WAIT(testMonitor);
			passedMonitor++;
		}
	};

	Thread1 thread1;
	Thread2 thread2;
	Thread3 thread3;
	// all will block on monitor
	thread1.start();
	thread2.start();
	thread3.start();
	Thread::sleepMs(20); // give threads a chance to run into wait
	if(passedMonitor != 0) {
		LOG_ERR("%d threads already passed the monitor", passedMonitor);
		assert(false);
	}
	UMUNDO_SIGNAL(testMonitor);
	Thread::sleepMs(20); // threads will increase passedMonitor
	if(passedMonitor != 3) {
		LOG_ERR("Expected 3 threads to pass the monitor, but %d did", passedMonitor);
		assert(false);
	}
	if (thread1.isStarted() || thread2.isStarted() || thread3.isStarted()) {
		LOG_ERR("Threads ran to completion but still insist on being started");
		assert(false);
	}
	// redo to check win32 monitor resetting
	thread1.start();
	thread2.start();
	thread3.start();
	Thread::sleepMs(20);
	if(passedMonitor != 3) {
		LOG_ERR("%d threads already passed the monitor", passedMonitor);
		assert(false);
	}
	UMUNDO_SIGNAL(testMonitor);
	Thread::sleepMs(20); // threads will increase passedMonitor
	if(passedMonitor != 6) {
		LOG_ERR("Expected 6 threads to pass the monitor, but %d did", passedMonitor);
		assert(false);
	}

	return true;
}

int main(int argc, char** argv) {
	if(!testRecursiveMutex())
		return EXIT_FAILURE;
	if(!testThreads())
		return EXIT_FAILURE;
	if(!testMonitors())
		return EXIT_FAILURE;
}

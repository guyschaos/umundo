#ifndef PTHREAD_H_KU2YWI3W
#define PTHREAD_H_KU2YWI3W

#include "common/stdInc.h"

#ifdef THREAD_PTHREAD
#include <pthread.h>
#include <errno.h>
#endif
#ifdef THREAD_WIN32
#include <windows.h>
#endif

namespace umundo {

class Thread {
public:
	Thread();
	virtual ~Thread();
	virtual void run() = 0;
	void join();
	void start();
	void stop();
	bool isStarted() {
		return _isStarted;
	}

	static void sleepMs(uint32_t ms);

protected:
	bool _isStarted;

#ifdef THREAD_PTHREAD
	static void* runWrapper(void*);
	pthread_t _thread;
#endif
#ifdef THREAD_WIN32
	static DWORD runWrapper(Thread *t);
	HANDLE _thread;
#endif

};

class Mutex {
public:
	Mutex();
	virtual ~Mutex();

	void lock();
	bool tryLock();
	void unlock();

private:
#ifdef THREAD_PTHREAD
	pthread_mutex_t _mutex;
#endif
#ifdef THREAD_WIN32
	HANDLE _mutex;
#endif

};

}

#endif /* end of include guard: PTHREAD_H_KU2YWI3W */

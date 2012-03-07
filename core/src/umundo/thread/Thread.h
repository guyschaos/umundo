#ifndef PTHREAD_H_KU2YWI3W
#define PTHREAD_H_KU2YWI3W

#include "umundo/common/Common.h"

//this is a hack until we get a compiler firewall per Pimpl
#ifdef _WIN32
# if !(defined THREAD_PTHREAD || defined THREAD_WIN32)
#   define THREAD_WIN32 1
# endif
#else
# if !(defined THREAD_PTHREAD || defined THREAD_WIN32)
#   define THREAD_PTHREAD 1
# endif
#endif

#if !(defined THREAD_PTHREAD || defined THREAD_WIN32)
#error No thread implementation choosen
#endif

#ifdef THREAD_PTHREAD
#include <pthread.h>
#include <errno.h>
#endif
#ifdef THREAD_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

namespace umundo {

/**
 * Platform independant parallel control-flows.
 */
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

	static void yield();
	static void sleepMs(uint32_t ms);

private:
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

/**
 * Platform independant mutual exclusion.
 */
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

/**
 * See comments from Schmidt on condition variables in windows:
 * http://www.cs.wustl.edu/~schmidt/win32-cv-1.html (we choose 3.2)
 */
class Monitor {
public:
	Monitor();
	virtual ~Monitor();

	void signal();
	bool wait() {
		return wait(0);
	}
	bool wait(uint32_t ms);

private:
#ifdef THREAD_PTHREAD
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
	bool _signaled;
#endif
#ifdef THREAD_WIN32
	int _waiters;
	Mutex _monitorLock;
	HANDLE _monitor;
#endif

};

}

#endif /* end of include guard: PTHREAD_H_KU2YWI3W */

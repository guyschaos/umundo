#ifndef PTHREAD_H_KU2YWI3W
#define PTHREAD_H_KU2YWI3W

#include "umundo/common/Common.h"

// this is a hack until we get a compiler firewall per Pimpl
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

#ifdef DEBUG_THREADS
#define UMUNDO_SCOPE_LOCK(mutex) \
LOG_DEBUG("Locking mutex %p", &mutex); \
ScopeLock lock(mutex);

#define UMUNDO_LOCK(mutex) \
LOG_DEBUG("Locking mutex %p", &mutex); \
mutex.lock(); \
LOG_DEBUG("Locked mutex %p", &mutex);

#define UMUNDO_TRYLOCK(mutex) \
LOG_DEBUG("Tying to lock mutex %p", &mutex); \
mutex.trylock();

#define UMUNDO_UNLOCK(mutex) \
LOG_DEBUG("Unlocking mutex %p", &mutex); \
mutex.unlock();

#define UMUNDO_WAIT(monitor) \
LOG_DEBUG("Waiting at monitor %p", &monitor); \
monitor.wait(); \
LOG_DEBUG("Signaled at monitor %p", &monitor);

#define UMUNDO_SIGNAL(monitor) \
LOG_DEBUG("Signaling monitor %p", &monitor); \
monitor.signal();
#endif

#ifndef DEBUG_THREADS
#define UMUNDO_SCOPE_LOCK(mutex) ScopeLock lock(mutex);
#define UMUNDO_LOCK(mutex) mutex.lock();
#define UMUNDO_TRYLOCK(mutex) mutex.tryLock();
#define UMUNDO_UNLOCK(mutex) mutex.unlock();
#define UMUNDO_WAIT(monitor) monitor.wait();
#define UMUNDO_SIGNAL(monitor) monitor.signal();
#endif

namespace umundo {

/**
 * Platform independent parallel control-flows.
 */
class Thread {
public:
	Thread();
	virtual ~Thread();
	virtual void run() = 0;
	virtual void join();
	void start();
	void stop();
	bool isStarted() {
		return _isStarted;
	}

	static void yield();
	static void sleepMs(uint32_t ms);
	static int getThreadId(); ///< integer unique to the current thread

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
 * Instantiate on stack to give code in scope below exclusive access.
 */
class ScopeLock {
public:
	ScopeLock(Mutex&);
	~ScopeLock();

	Mutex _mutex;
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

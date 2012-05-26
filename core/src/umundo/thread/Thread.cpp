#include "umundo/config.h"
#include "umundo/common/Debug.h"
#include "umundo/thread/Thread.h"

#if defined(UNIX) || defined(IOS)
#include <sys/time.h> // gettimeofday
#endif

namespace umundo {

Thread::Thread() {
	_isStarted = false;
}

Thread::~Thread() {
	if (_isStarted) {
		stop();
		join();
	}

#ifdef THREAD_PTHREAD
	pthread_detach(_thread);
#endif
#ifdef THREAD_WIN32
	CloseHandle(_thread);
#endif

}

void Thread::join() {
#ifdef THREAD_PTHREAD
	int err = 0;
	err = pthread_join(_thread, NULL);
	switch (err) {
	case EDEADLK:
		LOG_ERR("join: deadlock detected");
		break;
	case EINVAL:
		LOG_ERR("join: trying to join unjoinable thread");
		break;
	case ESRCH:
		LOG_ERR("join: no such thread");
		break;
	default:
		break;
	}
#endif
#ifdef THREAD_WIN32
	DWORD dwCode;
	dwCode = WaitForSingleObject(_thread, INFINITE);
	assert(dwCode==WAIT_OBJECT_0);
	GetExitCodeThread(_thread, &dwCode);
	assert(dwCode != STILL_ACTIVE);
#endif

}

int Thread::getThreadId() {
	static int _nextThreadId = 1;
	static Mutex _threadIdMutex;
#ifdef THREAD_PTHREAD
	static std::map<pthread_t, int> ids;
	pthread_t pt = pthread_self();
#endif
#ifdef THREAD_WIN32
	static std::map<DWORD, int> ids;
	DWORD pt = GetCurrentThreadId();
#endif
	_threadIdMutex.lock();
	if (ids.find(pt) == ids.end()) {
		ids[pt] = _nextThreadId++;
	}
	_threadIdMutex.unlock();
	return ids[pt];
}

void Thread::start() {
	if (_isStarted)
		return;
	_isStarted = true;

#ifdef THREAD_PTHREAD
	int err = pthread_create(&_thread, NULL, &runWrapper, (void*)this);
	(err == 0) || LOG_WARN("pthread_create failed");
#endif
#ifdef THREAD_WIN32
	DWORD threadId;
	_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runWrapper, this, 0, &threadId);
	assert(_thread != NULL);
#endif
}

#ifdef THREAD_PTHREAD
void* Thread::runWrapper(void *obj) {
	Thread* t = (Thread*)obj;
	t->run();
	t->_isStarted = false;
	return NULL;
}
#endif
#ifdef THREAD_WIN32
DWORD Thread::runWrapper(Thread *t) {
	t->run();
	t->_isStarted = false;
	return 0;
}
#endif

void Thread::stop() {
	_isStarted = false;
}

void Thread::yield() {
#ifdef THREAD_PTHREAD
	int err = sched_yield();
	(void)err;
	assert(!err);
#endif
#ifdef THREAD_WIN32
	SwitchToThread();
#endif
}

void Thread::sleepMs(uint32_t ms) {
#ifdef THREAD_PTHREAD
	usleep(1000 * ms);
#endif
#ifdef THREAD_WIN32
	Sleep(ms);
#endif
}

uint64_t Thread::getTimeStampMs() {
  uint64_t time = 0;
#ifdef WIN32
  FILETIME tv;
  GetSystemTimeAsFileTime(&tv);
  time = (((uint64_t) tv.dwHighDateTime) << 32) + tv.dwLowDateTime;
  time /= 10000;
#endif
#ifdef UNIX
  struct timeval tv;
	gettimeofday(&tv, NULL);
  time += tv.tv_sec * 1000;
  time += tv.tv_usec / 1000;
#endif
  return time;
}

  
Mutex::Mutex() {
#ifdef THREAD_PTHREAD
	pthread_mutexattr_t attrib;
	int ret = pthread_mutexattr_init(&attrib);
	(void)ret;
	assert(ret == 0);
	ret = pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE);
	assert(ret == 0);
	pthread_mutex_init(&_mutex, &attrib);
	pthread_mutexattr_destroy(&attrib);
#endif
#ifdef THREAD_WIN32
	_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
}

Mutex::~Mutex() {
#ifdef THREAD_PTHREAD
	pthread_mutex_destroy(&_mutex);
#endif
#ifdef THREAD_WIN32
	CloseHandle(_mutex);
#endif

}

void Mutex::lock() {
#ifdef THREAD_PTHREAD
	pthread_mutex_lock(&_mutex);
#endif
#ifdef THREAD_WIN32
	WaitForSingleObject(_mutex, INFINITE);
#endif
}

bool Mutex::tryLock() {
#ifdef THREAD_PTHREAD
	if(pthread_mutex_trylock(&_mutex) == EBUSY)
		return false;
#endif
#ifdef THREAD_WIN32
	if(WaitForSingleObject(_mutex, 0) == WAIT_TIMEOUT)
		return false;
#endif
	return true;
}

void Mutex::unlock() {
#ifdef THREAD_PTHREAD
	pthread_mutex_unlock(&_mutex);
#endif
#ifdef THREAD_WIN32
	ReleaseMutex(_mutex);
#endif

}

ScopeLock::ScopeLock(Mutex* mutex) : _mutex(mutex) {
	_mutex->lock();
}

ScopeLock::~ScopeLock() {
	_mutex->unlock();
}

Monitor::Monitor() {
#ifdef THREAD_PTHREAD
	int err;
	err = pthread_mutex_init(&_mutex, NULL);
	assert(err == 0);
	err = pthread_cond_init(&_cond, NULL);
	assert(err == 0);
	(void)err; // avoid unused warning
	_signaled = false;
#endif
#ifdef THREAD_WIN32
	_waiters = 0;
	_monitor = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
}

Monitor::~Monitor() {
#ifdef THREAD_PTHREAD
	int err;
	while((err = pthread_cond_destroy(&_cond))) {
		switch(err) {
		case EBUSY:
			LOG_WARN("Trying to destroy locked monitor - retrying");
			break;
		default:
			LOG_ERR("pthread_mutex_destroy returned %d");
			goto failed_cond_destroy;
		}
		Thread::sleepMs(50);
	}
failed_cond_destroy:
	while((err = pthread_mutex_destroy(&_mutex))) {
		switch(err) {
		case EBUSY:
			LOG_WARN("Trying to destroy locked monitor - retrying");
			break;
		default:
			LOG_ERR("pthread_mutex_destroy returned %d");
			goto failed_mutex_destroy;
		}
		Thread::sleepMs(50);
	}
failed_mutex_destroy:
	return;
#endif
#ifdef THREAD_WIN32
	CloseHandle(_monitor);
#endif
}

void Monitor::signal() {
#ifdef THREAD_PTHREAD
	pthread_mutex_lock(&_mutex);
	_signaled = true;
	pthread_cond_broadcast(&_cond);
	pthread_mutex_unlock(&_mutex);
#endif
#ifdef THREAD_WIN32
	_monitorLock.lock();
	bool somonesWaiting = _waiters > 0;
	_monitorLock.unlock();

	if (somonesWaiting)
		SetEvent(_monitor);
#endif
}

bool Monitor::wait(uint32_t ms) {
#ifdef THREAD_PTHREAD
	int rv;
	pthread_mutex_lock(&_mutex);
	if (_signaled) {
		//LOG_DEBUG("Signaled prior to waiting");
		_signaled = false;
		pthread_mutex_unlock(&_mutex);
		return true;
	}
	// wait indefinitely
	if (ms == 0) {
    while(!_signaled)
      rv = pthread_cond_wait(&_cond, &_mutex);
		if (rv == 0)
			_signaled = false;
		pthread_mutex_unlock(&_mutex);
		return rv == 0;
	}

	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv.tv_usec += (ms % 1000) * 1000;
	tv.tv_sec += (ms / 1000) + (tv.tv_usec / 1000000);
	tv.tv_usec %= 1000000;
	struct timespec ts;
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	rv = pthread_cond_timedwait(&_cond, &_mutex, &ts);
	if (rv == 0)
		_signaled = false;
	pthread_mutex_unlock(&_mutex);
	if (rv != 0 && rv != ETIMEDOUT)
		assert(false);
	return rv == 0;
#endif
#ifdef THREAD_WIN32
	_monitorLock.lock();
	_waiters++;
	_monitorLock.unlock();
	if (ms == 0)
		ms = INFINITE;
	int result = WaitForSingleObject(_monitor, ms);
	_monitorLock.lock();
	_waiters--;
	int last_waiter =
	    result == WAIT_OBJECT_0 && _waiters == 0;
	_monitorLock.unlock();

	if (last_waiter)
		ResetEvent (_monitor);
	return result == WAIT_OBJECT_0;
#endif
}

#ifdef THREAD_PTHREAD
#endif
#ifdef THREAD_WIN32
#endif

}
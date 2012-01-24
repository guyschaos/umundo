#include "thread/Thread.h"

namespace umundo {

Thread::Thread() {
	DEBUG_CTOR("Thread");
	_isStarted = false;
}

Thread::~Thread() {
	DEBUG_DTOR("Thread");
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
	assert(err == 0);
#endif
#ifdef THREAD_WIN32
  DWORD dwCode;
  dwCode = WaitForSingleObject(_thread, INFINITE);
	assert(dwCode==WAIT_OBJECT_0);
	GetExitCodeThread(_thread, &dwCode);
	assert(dwCode != STILL_ACTIVE);
#endif

}

void Thread::start() {
	if (_isStarted)
		return;
	_isStarted = true;
	
#ifdef THREAD_PTHREAD	
	int err = pthread_create(&_thread, NULL, &runWrapper, (void*)this);
	assert(err == 0);
#endif
#ifdef THREAD_WIN32
  DWORD threadId;
  _thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runWrapper, this, 0, &threadId);
	assert(_thread != NULL);
#endif
}

#ifdef THREAD_PTHREAD	
void* Thread::runWrapper(void *obj) {
	Thread* myself = (Thread*)obj;
	myself->run();
	return NULL;
}
#endif
#ifdef THREAD_WIN32
DWORD Thread::runWrapper(Thread *t) {
  t->run();
  return 0;
}
#endif

void Thread::stop() {
	_isStarted = false;
}

void Thread::sleepMs(uint32_t ms) {
#ifdef THREAD_PTHREAD	
	usleep(1000 * ms);
#endif
#ifdef THREAD_WIN32
	Sleep(1000 * ms);
#endif
}

Mutex::Mutex() {
	DEBUG_CTOR("Mutex");
#ifdef THREAD_PTHREAD	
	pthread_mutexattr_t attrib;
	int ret = pthread_mutexattr_init(&attrib);
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
	DEBUG_DTOR("Mutex");
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

#ifdef THREAD_PTHREAD	
#endif
#ifdef THREAD_WIN32
#endif

}
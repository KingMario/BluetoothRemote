#include "Thread.h"
#include <iostream>
#include <pthread.h>

using namespace std;

Thread::Thread() :
retVal(0), _joinable(true), _runs(false)
{
	init();
}

Thread::Thread(bool joinable) :
retVal(0), _joinable(joinable), _runs(false)
{
	init();
}

Thread::~Thread()
{
	if (pthread_attr_destroy(&thAtt) != 0) {
		throw Thread_exception("cannot destroy thread.");
	}
}

void Thread::init()
{
	if (pthread_attr_init(&thAtt) != 0) {
		throw Thread_exception("cannot init thread attributes");
	}
	if (pthread_attr_setdetachstate(&thAtt,
			(_joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED))) {
		throw Thread_exception("cannot set thread attribute.");
	}
}

void Thread::start()
{
	runsLock.lock();
	if (pthread_create(&th, &thAtt, &Thread::_start, this) != 0) {
		runsLock.unlock();
		throw Thread_exception("cannot start new thread.");
	}
	_runs = true;
	runsLock.unlock();
}

void Thread::join()
{
	runsLock.lock();
	if (_runs) {
		if (pthread_join(th, &retVal) != 0) {
			runsLock.unlock();
			throw Thread_exception("cannot join thread.");
		}
	}
	runsLock.unlock();
}

void * Thread::_start(void*o)
{
	Thread * t = (Thread*) o;
	t->run();
	t->_runs = false;
	pthread_exit(0);
}

bool Thread::joinable()
{
	return _joinable;
}

bool Thread::runs()
{
	runsLock.lock();
	bool r = _runs;
	runsLock.unlock();
	return r;
}

void Thread::runs(bool r)
{
	runsLock.lock();
	_runs = r;
	runsLock.unlock();
}
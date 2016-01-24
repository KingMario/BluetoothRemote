#include "Thread.h"

#include <pthread.h>

Thread::Thread() :
    _joinable(true)
{
    init();
}

Thread::Thread(bool joinable) :
    _joinable(joinable)
{
    init();
}

Thread::~Thread()
{
    pthread_attr_destroy(&thAtt);
}

void Thread::init()
{

    pthread_attr_init(&thAtt);
    pthread_attr_setdetachstate(&thAtt,
                                (_joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED));

    runs(false);
}

void Thread::start()
{
    runsLock.lock();
    pthread_create(&th, &thAtt, &Thread::_start, this);
    _runs = true;
    runsLock.unlock();
}

void Thread::join()
{
    if(_joinable && runs())
    {
        pthread_join(th, NULL);
        runs(false);
    }
}

void * Thread::_start(void*o)
{
    Thread * t = (Thread*)o;
    t->run();
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

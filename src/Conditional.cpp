#include "Conditional.h"
#include <iostream>

#include <pthread.h>

using namespace std;

class Conditional_Exception { };

Conditional::Conditional()
{
    init();
}

Conditional::~Conditional()
{
    if(pthread_cond_destroy(&cond))
    {
        throw Conditional_Exception();
    }
}

void Conditional::init()
{
    if(pthread_cond_init(&cond, NULL)!=0)
    {
        throw Conditional_Exception();
    }
}

void Conditional::wait()
{
    if(pthread_cond_wait(&cond, &mux)!=0)
    {
        throw Conditional_Exception();
    }
}

void Conditional::signal()
{
    if(pthread_cond_signal(&cond)!=0)
    {
        throw Conditional_Exception();
    }
}

void Conditional::signalAll()
{
    if(pthread_cond_broadcast(&cond)!=0)
    {
        throw Conditional_Exception();
    }
}

void Conditional::lock()
{
    try
    {
        Lockable::lock();
    }
    catch (const Lock_Exception &le)
    {
        throw Conditional_Exception();
    }
}
void Conditional::unlock()
{
    try
    {
        Lockable::unlock();
    }
    catch (const Lock_Exception &le)
    {
        throw Conditional_Exception();
    }
}

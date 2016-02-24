#ifndef _THREAD_H_
#define _THREAD_H_
#include <pthread.h>

#include "Lockable.h"
#include "Thread_exception.h"

class Thread {
    
    pthread_t th;
    pthread_attr_t thAtt;
    
    void *retVal;

    bool _joinable;
    bool _runs;
    Lockable runsLock;

    void init();

    bool runs();
    void runs(bool);

    static void * _start(void*);

public:
    Thread();
    Thread(bool);
    virtual ~Thread();
    virtual void run() = 0;
    void join();
    void start();
    bool joinable();
};

#endif
#ifndef _THREAD_H_
#define _THREAD_H_
#include <pthread.h>

#include "Lockable.h"

class Thread
{

    pthread_t th;
    pthread_attr_t thAtt;

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

    ~Thread();

    virtual void run() = 0;

    void join();
    void start();

    bool joinable();
};

#endif

#ifndef _CONDITIONAL_H_
#define _CONDITIONAL_H_

#include "Lockable.h"

class Conditional : public Lockable
{
    pthread_cond_t cond;
    void init();
public:

    Conditional();
    ~Conditional();

    void lock();
    void unlock();

    void signal();
    void signalAll();

    void wait();

};

#endif

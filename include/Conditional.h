#ifndef _CONDITIONAL_H_
#define _CONDITIONAL_H_

#include "Lockable.h"
#include "Thread_exception.h"

typedef unsigned long long int uint64;

class Conditional : public Lockable {
    pthread_cond_t cond;
    void init();
public:

    Conditional();
    virtual ~Conditional();

    void signal();
    void signalAll();

    void wait();
    void wait(uint64 milliseconds);
};

#endif
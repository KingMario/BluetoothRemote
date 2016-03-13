#ifndef _LOCKABLE_H_
#define _LOCKABLE_H_
#include <pthread.h>
#include "Thread_exception.h"

class Lockable {
    friend class Conditional;
public:
    Lockable();
    virtual ~Lockable();
    void lock();
    void unlock();
private:
    pthread_mutex_t mux;
};

#endif
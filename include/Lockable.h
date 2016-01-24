#ifndef _LOCKABLE_H_
#define _LOCKABLE_H_
#include <pthread.h>

class Lock_Exception {  };

class Lockable
{
    friend class Conditional;
public:
    Lockable();
    ~Lockable();
    void lock();
    void unlock();
private:
    pthread_mutex_t mux;
};

#endif

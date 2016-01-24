#ifndef CONSUMER_H
#define CONSUMER_H

#include "Thread.h"
#include "Conditional.h"
#include "Lockable.h"
#include <queue>

template<class T>
class Consumer : public Thread, public Conditional {

    std::queue<T> pending;
    std::queue<T> proccesing;

    bool r;
    Lockable runningLock;

public:

    Consumer() : r(true) {
    }

    void add(T item) {
        lock();
        pending.push(item);
        signal();
        unlock();
    }

    void run() {
        while(running()) {
            lock();
            while(pending.empty() && running()) {
                wait();
            }
            while(!pending.empty()) {
                proccesing.push(pending.front());
                pending.pop();
            }
            unlock();

            while(!proccesing.empty()) {
                T &item = proccesing.front();
                proccess(item);
                proccesing.pop();
            }

        }
    }

    bool running() {
        runningLock.lock();
        bool r = this->r;
        runningLock.unlock();
        return r;
    }

    void shutdown() {
        runningLock.lock();
        this->r = false;
        runningLock.unlock();
        lock();
        signal();
        unlock();
    }

    virtual void proccess(T &item) = 0;
};

#endif // CONSUMER_H

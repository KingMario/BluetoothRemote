#include "Conditional.h"
#include <errno.h>
#include <pthread.h>

Conditional::Conditional()
{
	init();
}

Conditional::~Conditional()
{
	if (pthread_cond_destroy(&cond) != 0) {
		throw Thread_exception("cannot destroy conditional.");
	}
}

void Conditional::init()
{
	if (pthread_cond_init(&cond, NULL) != 0) {
		throw Thread_exception("cannot init conditional.");
	}
}

void Conditional::wait()
{
	if (pthread_cond_wait(&cond, &mux) != 0) {
		throw Thread_exception("cannot perform wait.");
	}
}

void Conditional::wait(uint64 milliseconds) {
    timespec timewait;
    clock_gettime(CLOCK_REALTIME, &timewait);
    timewait.tv_sec += milliseconds/1000;
    timewait.tv_nsec += (1000*1000)*(milliseconds%1000);
    int rc = 0;
    while(true) {
        rc = pthread_cond_timedwait(&cond, &mux, &timewait);
        if(rc == 0 || rc == ETIMEDOUT) {
            break;
        } else {
            throw Thread_exception("cannot perform wait(milliseconds).");
        }
    }
}

void Conditional::signal()
{
	if (pthread_cond_signal(&cond) != 0) {
		throw Thread_exception("cannot signal wait.");
	}
}

void Conditional::signalAll()
{
	if (pthread_cond_broadcast(&cond) != 0) {
		throw Thread_exception("cannot signalAll wait.");
	}
}
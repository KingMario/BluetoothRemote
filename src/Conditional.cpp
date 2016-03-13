#include "Conditional.h"

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
#include "Lockable.h"

#include <pthread.h>

Lockable::Lockable()
{
	if (pthread_mutex_init(&mux, NULL) != 0) {
		throw Thread_exception("cannot init mutex.");
	}
}

Lockable::~Lockable()
{
	if (pthread_mutex_destroy(&mux) != 0) {
		throw Thread_exception("cannot destroy mutex.");
	}
}

void Lockable::lock()
{
	if (pthread_mutex_lock(&mux) != 0) {
		throw Thread_exception("cannot lock mutex.");
	}
}

void Lockable::unlock()
{
	if (pthread_mutex_unlock(&mux) != 0) {
		throw Thread_exception("cannot unlock mutex.");
	}
}
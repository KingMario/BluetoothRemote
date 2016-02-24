#ifndef THREAD_EXCEPTION_H
#define THREAD_EXCEPTION_H

struct Thread_exception {
private:
    const char * msg;
public:

    Thread_exception(const char *msg) :
    msg(msg) {
    }

    const char * message() const {
	return msg;
    }
};

#endif /* THREAD_EXCEPTION_H */


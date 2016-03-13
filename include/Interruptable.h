#ifndef INTERRUPTABLE_H
#define INTERRUPTABLE_H

#include "Log.h"
#include "Thread.h"
#include "Lockable.h"

class Interruptable : public Thread, Lockable {
public:

    Interruptable() : _interrupted(false) {

    }

    void interrupted(bool interrupted) {
	Log::logMsg("Interruptable::interrupted(" + std::string((interrupted ? "true" : "false")) + ")");
	lock();
	_interrupted = interrupted;
	unlock();
    }

private:
    bool _interrupted;

protected:

    bool interrupted() {
	lock();
	bool interrupted = _interrupted;
	unlock();
	return interrupted;
    }
};

#endif /* INTERRUPTABLE_H */


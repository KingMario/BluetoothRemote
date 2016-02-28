#ifndef LOG_H
#define LOG_H

class Log : Lockable {
	std::ostream &out;
	Log() : out(std::cout) {
		
	}
	static Log& getInstance() {
		static Log log;
		return log;
	}
	void log(const std::string &msg) {
		lock();
		out << msg << std::endl;
		unlock();
	}
public:
	static void logMsg(const std::string &msg) {
		Log::getInstance().log(msg);
	}
};

#endif /* LOG_H */


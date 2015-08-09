/*
 * FLogger.h
 *
 *  Created on: 2015年7月15日
 *      Author: aka
 */

#ifndef FCORE_FLOGGER_H_
#define FCORE_FLOGGER_H_

#include "FLoggerMacro.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <pthread.h>

namespace freeyxm {

enum class LogLevel {
	DEBUG,
	INFO,
	WARN,
	ERROR,
};

class FLogger {

public:
	FLogger();
	virtual ~FLogger();

	static void log(LogLevel level, const char *fmt, ...);
	static void log_t(LogLevel level, const char *fmt, ...);

	static void logm(LogLevel level, const char *fmt, ...);
	static void logm_t(LogLevel level, const char *fmt, ...);

	inline static void log_lock();
	inline static void log_unlock();

	inline static FILE* getOutStream();

	static LogLevel log_level;

private:
	static pthread_mutex_t log_mutex;
	static FILE* out_stream;
};

inline void FLogger::log_lock()
{
	pthread_mutex_lock(&log_mutex);
}

inline void FLogger::log_unlock()
{
	pthread_mutex_unlock(&log_mutex);
}

inline FILE* FLogger::getOutStream()
{
	return out_stream;
}

} /* namespace freeyxm */

#endif /* FCORE_FLOGGER_H_ */

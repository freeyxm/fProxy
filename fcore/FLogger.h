/*
 * FLogger.h
 *
 *  Created on: 2015年7月15日
 *      Author: aka
 */

#ifndef FCORE_FLOGGER_H_
#define FCORE_FLOGGER_H_

#include <cstdio>
#include <pthread.h>

namespace freeyxm {

#ifdef _DEBUG_
#define DEBUG_PRINT(mutex, fmt, ...) {\
	if(mutex)\
		FLogger::mlog(fmt, ##__VA_ARGS__);\
	else\
		FLogger::log(fmt, ##__VA_ARGS__);\
}
#define DEBUG_PRINT_T(mutex, fmt, ...) {\
	if(mutex)\
		FLogger::mlog_t(fmt, ##__VA_ARGS__);\
	else\
		FLogger::log_t(fmt, ##__VA_ARGS__);\
}
#else
#define DEBUG_PRINT(mutex, fmt, ...) ;
#define DEBUG_PRINT_T(mutex, fmt, ...) ;
#endif

#define DEBUG_PRINT_LOCK() FLogger::log_lock()
#define DEBUG_PRINT_UNLOCK() FLogger::log_unlock()

#define DEBUG_PRINTLN_MSG(msg) DEBUG_PRINT_T(1,"%s\n",msg)
#define DEBUG_PRINTLN_FL(msg) DEBUG_PRINT_T(1,"%s[%d]: %s\n",__FILE__,__LINE__,msg)
#define DEBUG_PRINTLN_ERR(msg,errcode,errstr) DEBUG_PRINT_T(1,"%s, ERR[%d]: %s\n",msg,errcode,errstr)

class FLogger {
public:
	FLogger();
	virtual ~FLogger();

	static void log(const char *fmt, ...);
	static void log_t(const char *fmt, ...);

	static void mlog(const char *fmt, ...);
	static void mlog_t(const char *fmt, ...);

	static void log_lock();
	static void log_unlock();

private:
	static pthread_mutex_t log_mutex;
	static FILE* out_stream;
};

} /* namespace freeyxm */

#endif /* FCORE_FLOGGER_H_ */

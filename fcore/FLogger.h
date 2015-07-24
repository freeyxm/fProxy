/*
 * FLogger.h
 *
 *  Created on: 2015年7月15日
 *      Author: aka
 */

#ifndef FCORE_FLOGGER_H_
#define FCORE_FLOGGER_H_

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <pthread.h>

namespace freeyxm {

#define LOG_PRINT(fmt, ...) {\
	fprintf(FLogger::getOutStream(), fmt, ##__VA_ARGS__);\
	fflush(FLogger::getOutStream());\
}

#define LOG_PRINT_T(fmt, ...) {\
	char str[20];\
	time_t lt = time(NULL);\
	struct tm *ptm = localtime(&lt);\
	strftime(str, sizeof(str), "%F %T", ptm);\
	fprintf(FLogger::getOutStream(), "[%s] ", str);\
	LOG_PRINT(fmt, ##__VA_ARGS__);\
}

#define MLOG_PRINT(fmt, ...) {\
	FLogger::log_lock();\
	LOG_PRINT(fmt, ##__VA_ARGS__);\
	FLogger::log_unlock();\
}

#define MLOG_PRINT_T(fmt, ...) {\
	FLogger::log_lock();\
	LOG_PRINT_T(fmt, ##__VA_ARGS__);\
	FLogger::log_unlock();\
}

#ifdef _DEBUG_
#define DEBUG_PRINT(fmt, ...) LOG_PRINT(fmt, ##__VA_ARGS__)
#define DEBUG_PRINT_T(fmt, ...) LOG_PRINT_T(fmt, ##__VA_ARGS__)
#define DEBUG_MPRINT(fmt, ...) MLOG_PRINT(fmt, ##__VA_ARGS__)
#define DEBUG_MPRINT_T(fmt, ...) MLOG_PRINT_T(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) ;
#define DEBUG_PRINT_T(fmt, ...) ;
#define DEBUG_MPRINT(fmt, ...) ;
#define DEBUG_MPRINT_T(fmt, ...) ;
#endif

#define DEBUG_PRINT_LOCK() FLogger::log_lock()
#define DEBUG_PRINT_UNLOCK() FLogger::log_unlock()

#define DEBUG_PRINTLN_MSG(msg) DEBUG_MPRINT_T("%s\n",msg)
#define DEBUG_PRINTLN_FL(msg) DEBUG_MPRINT_T("%s[%d]: %s\n",__FILE__,__LINE__,msg)
#define DEBUG_PRINTLN_ERR(msg,errcode,errstr) DEBUG_MPRINT_T("%s, ERR[%d]: %s\n",msg,errcode,errstr)

#define LOG_PRINTLN_MSG(msg) FLogger::mlog_t("%s\n",msg)
#define LOG_PRINTLN_FL(msg) FLogger::mlog_t("%s[%d]: %s\n",__FILE__,__LINE__,msg)
#define LOG_PRINTLN_ERR(msg,errcode,errstr) FLogger::mlog_t("%s, ERR[%d]: %s\n",msg,errcode,errstr)

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

	static FILE* getOutStream();

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

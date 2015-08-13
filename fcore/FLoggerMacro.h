/*
 * FLoggerMacro.h
 *
 *  Created on: 2015年8月9日
 *      Author: aka
 */

#ifndef FCORE_FLOGGERMACRO_H_
#define FCORE_FLOGGERMACRO_H_

namespace freeyxm {

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

#define LOG_PRINT_LOCK() FLogger::log_lock()
#define LOG_PRINT_UNLOCK() FLogger::log_unlock()

#define LOG_PRINT(fmt, ...) {\
	::fprintf(FLogger::getOutStream(), fmt, ##__VA_ARGS__);\
	::fflush(FLogger::getOutStream());\
}

#define LOG_PRINT_T(fmt, ...) {\
	char str[20];\
	time_t lt = ::time(NULL);\
	struct tm *ptm = ::localtime(&lt);\
	::strftime(str, sizeof(str), "%F %T", ptm);\
	::fprintf(FLogger::getOutStream(), "[%s] ", str);\
	LOG_PRINT(fmt, ##__VA_ARGS__);\
}

#define LOGM_PRINT(fmt, ...) {\
	LOG_PRINT_LOCK();\
	LOG_PRINT(fmt, ##__VA_ARGS__);\
	LOG_PRINT_UNLOCK();\
}

#define LOGM_PRINT_T(fmt, ...) {\
	LOG_PRINT_LOCK();\
	LOG_PRINT_T(fmt, ##__VA_ARGS__);\
	LOG_PRINT_UNLOCK();\
}

#if defined(LOG_LEVEL) && LOG_LEVEL <= LOG_LEVEL_ERROR
#define ELOG_PRINT(fmt, ...) LOG_PRINT(fmt, ##__VA_ARGS__)
#define ELOG_PRINT_T(fmt, ...) LOG_PRINT_T(fmt, ##__VA_ARGS__)
#define ELOGM_PRINT(fmt, ...) LOGM_PRINT(fmt, ##__VA_ARGS__)
#define ELOGM_PRINT_T(fmt, ...) LOGM_PRINT_T(fmt, ##__VA_ARGS__)
#define ELOGM_PRINTLN_MSG(msg) LOGM_PRINT_T("%s\n",msg)
#define ELOGM_PRINTLN_FL(msg) LOGM_PRINT_T("%s[%d]: %s\n",__FILE__,__LINE__,msg)
#define ELOGM_PRINTLN_ERR(msg,errcode,errstr) LOGM_PRINT_T("%s, ERR[%d]: %s\n",msg,errcode,errstr)
#else
#define ELOG_PRINT(fmt, ...) ;
#define ELOG_PRINT_T(fmt, ...) ;
#define ELOGM_PRINT(fmt, ...) ;
#define ELOGM_PRINT_T(fmt, ...) ;
#define ELOGM_PRINTLN_MSG(msg) ;
#define ELOGM_PRINTLN_FL(msg) ;
#define ELOGM_PRINTLN_ERR(msg,errcode,errstr) ;
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL <= LOG_LEVEL_DEBUG
#define DLOG_PRINT(fmt, ...) LOG_PRINT(fmt, ##__VA_ARGS__)
#define DLOG_PRINT_T(fmt, ...) LOG_PRINT_T(fmt, ##__VA_ARGS__)
#define DLOGM_PRINT(fmt, ...) LOGM_PRINT(fmt, ##__VA_ARGS__)
#define DLOGM_PRINT_T(fmt, ...) LOGM_PRINT_T(fmt, ##__VA_ARGS__)
#define DLOGM_PRINTLN_MSG(msg) LOGM_PRINT_T("%s\n",msg)
#define DLOGM_PRINTLN_FL(msg) LOGM_PRINT_T("%s[%d]: %s\n",__FILE__,__LINE__,msg)
#define DLOGM_PRINTLN_ERR(msg,errcode,errstr) LOGM_PRINT_T("%s, ERR[%d]: %s\n",msg,errcode,errstr)
#else
#define DLOG_PRINT(fmt, ...) ;
#define DLOG_PRINT_T(fmt, ...) ;
#define DLOGM_PRINT(fmt, ...) ;
#define DLOGM_PRINT_T(fmt, ...) ;
#define DLOGM_PRINTLN_MSG(msg) ;
#define DLOGM_PRINTLN_FL(msg) ;
#define DLOGM_PRINTLN_ERR(msg,errcode,errstr) ;
#endif

} /* namespace freeyxm */

#endif /* FCORE_FLOGGERMACRO_H_ */

/*
 * FUtil.h
 *
 *  Created on: 2012-10-26
 *      Author: freeyxm
 */

#ifndef FUTIL_H_
#define FUTIL_H_

#include <cstdio>
#include <cstdarg>
#include <ctime>
#ifdef _DEBUG_MUTEX_
#include <pthread.h>
#endif
#include <string>

namespace freeyxm {

using std::string;

#ifdef _DEBUG_MUTEX_
extern pthread_mutex_t debug_out_mutex;
#endif

inline void DEBUG_PRINT(int mutex, const char *fmt, ...) {
#ifdef _DEBUG_
	va_list argptr;
	va_start(argptr, fmt);
#ifdef _DEBUG_MUTEX_
	if (mutex) {
		pthread_mutex_lock(&debug_out_mutex);
	}
#endif
	vfprintf(stdout, fmt, argptr);
	fflush(stdout); // ...
#ifdef _DEBUG_MUTEX_
	if (mutex) {
		pthread_mutex_unlock(&debug_out_mutex);
	}
#endif
	va_end(argptr);
#endif
}

inline void DEBUG_PRINT_T(int mutex, const char *fmt, ...) {
#ifdef _DEBUG_
	va_list argptr;
	va_start(argptr, fmt);
#ifdef _DEBUG_MUTEX_
	if (mutex) {
		pthread_mutex_lock(&debug_out_mutex);
	}
#endif
	fprintf(stdout,"[%ld] ",time(NULL));
	vfprintf(stdout, fmt, argptr);
	fflush(stdout); // ...
#ifdef _DEBUG_MUTEX_
	if (mutex) {
		pthread_mutex_unlock(&debug_out_mutex);
	}
#endif
	va_end(argptr);
#endif
}

inline void DEBUG_PRINT_LOCK() {
#ifdef _DEBUG_MUTEX_
	pthread_mutex_lock(&debug_out_mutex);
#endif
}

inline void DEBUG_PRINT_UNLOCK() {
#ifdef _DEBUG_MUTEX_
	pthread_mutex_unlock(&debug_out_mutex);
#endif
}

#define DEBUG_PRINTLN_MSG(msg) DEBUG_PRINT_T(1,"%s\n",msg)
#define DEBUG_PRINTLN_FL(msg) DEBUG_PRINT_T(1,"%s[%d]: %s\n",__FILE__,__LINE__,msg)
#define DEBUG_PRINTLN_ERR(msg,errcode,errstr) DEBUG_PRINT_T(1,"%s, ERR[%d]: %s\n",msg,errcode,errstr)

class FUtil {
public:
	FUtil();
	virtual ~FUtil();

	static void print_hex(FILE *out, const unsigned char *bytes, const int size,
			const int line_num);

	static int getErrCode();
	static string getErrStr();
};

} /* namespace freeyxm */
#endif /* FUTIL_H_ */

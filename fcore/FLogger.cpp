/*
 * FLogger.cpp
 *
 *  Created on: 2015年7月15日
 *      Author: aka
 */

#include "FLogger.h"

#include <cstdarg>
#include <ctime>

namespace freeyxm {

pthread_mutex_t FLogger::log_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE* FLogger::out_stream = stdout;

FLogger::FLogger() {
	// TODO Auto-generated constructor stub
}

FLogger::~FLogger() {
	// TODO Auto-generated destructor stub
}

void FLogger::log(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);

	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	va_end(argptr);
}

void FLogger::log_t(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);

	fprintf(out_stream, "[%ld] ", time(NULL));
	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	va_end(argptr);
}

void FLogger::mlog(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	pthread_mutex_lock(&log_mutex);

	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	pthread_mutex_unlock(&log_mutex);
	va_end(argptr);
}

void FLogger::mlog_t(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	pthread_mutex_lock(&log_mutex);

	fprintf(out_stream, "[%ld] ", time(NULL));
	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	pthread_mutex_unlock(&log_mutex);
	va_end(argptr);
}

void FLogger::log_lock() {
	pthread_mutex_lock(&log_mutex);
}

void FLogger::log_unlock() {
	pthread_mutex_unlock(&log_mutex);
}

} /* namespace freeyxm */

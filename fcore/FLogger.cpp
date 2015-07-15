/*
 * FLogger.cpp
 *
 *  Created on: 2015年7月15日
 *      Author: aka
 */

#include "FLogger.h"

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

	char str[20];
	time_t lt = time(NULL);
	struct tm *ptm = localtime(&lt);
	strftime(str, sizeof(str), "%F %T", ptm);

	fprintf(out_stream, "[%s] ", str);
	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	va_end(argptr);
}

void FLogger::mlog(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	log_lock();

	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	log_unlock();
	va_end(argptr);
}

void FLogger::mlog_t(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	log_lock();

	char str[20];
	time_t lt = time(NULL);
	struct tm *ptm = localtime(&lt);
	strftime(str, sizeof(str), "%F %T", ptm);

	fprintf(out_stream, "[%s] ", str);
	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	log_unlock();
	va_end(argptr);
}

} /* namespace freeyxm */

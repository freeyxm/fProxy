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
LogLevel FLogger::log_level = LogLevel::DEBUG;

FLogger::FLogger()
{
}

FLogger::~FLogger()
{
}

void FLogger::log(LogLevel level, const char *fmt, ...)
{
	if (level < FLogger::log_level)
		return;

	va_list argptr;
	va_start(argptr, fmt);

	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	va_end(argptr);
}

void FLogger::log_t(LogLevel level, const char *fmt, ...)
{
	if (level < FLogger::log_level)
		return;

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

void FLogger::logm(LogLevel level, const char *fmt, ...)
{
	if (level < FLogger::log_level)
		return;

	va_list argptr;
	va_start(argptr, fmt);
	log_lock();

	vfprintf(out_stream, fmt, argptr);
	fflush(out_stream);

	log_unlock();
	va_end(argptr);
}

void FLogger::logm_t(LogLevel level, const char *fmt, ...)
{
	if (level < FLogger::log_level)
		return;

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

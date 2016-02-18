/*
 * FUtil.cpp
 *
 *  Created on: 2012-10-26
 *      Author: kill
 */

#include "FUtil.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#ifdef __WIN32__
#include <winbase.h>
#elif defined(__linux__)
#include <errno.h>
#endif

namespace freeyxm {

FUtil::FUtil()
{
}

FUtil::~FUtil()
{
}

void FUtil::print_hex(FILE *out, const unsigned char *bytes, const int size, const int line_num)
{
	int i, j;
	for (i = 0, j = 1; i < size; ++i, ++j)
	{
		fprintf(out, " %02x", (unsigned) bytes[i]);
		if (j == line_num)
		{
			fprintf(out, "\n");
			j = 0;
		}
	}
	if (line_num && j != 0)
	{
		fprintf(out, "\n");
	}
	fflush(out); // ...
}

int FUtil::getErrCode()
{
#ifdef __WIN32__
	return ::GetLastError();
#else
	return errno;
#endif
}

const char* FUtil::getErrStr(int errCode)
{
	const int BUF_SIZE = 128;
#ifdef __WIN32__
	static __declspec(thread) char buf[BUF_SIZE] = "";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, BUF_SIZE, NULL);
	return buf;
#else
	static __thread char buf[BUF_SIZE] = "";
#if ! _GNU_SOURCE
	int ret = ::strerror_r(errCode, buf, BUF_SIZE);
	if(ret != 0)
	{
		int errCode = ret > 0 ? ret : getErrCode();
		ELOGM_PRINTLN_ERR("strerror_r error", errCode, FUtil::getErrStr(errCode));
		return NULL;
	}
	return buf;
#else
	return ::strerror_r(errCode, buf, BUF_SIZE);
#endif
#endif
}

const char* FUtil::getErrStr()
{
#ifdef __WIN32__
	return FUtil::getErrStr(FUtil::getErrCode());
#else
	return FUtil::getErrStr(errno);
#endif
}

} /* namespace freeyxm */

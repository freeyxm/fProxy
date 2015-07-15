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
#elif __linux__
#include <errno.h>
#endif

namespace freeyxm {

FUtil::FUtil() {
	// TODO Auto-generated constructor stub
}

FUtil::~FUtil() {
	// TODO Auto-generated destructor stub
}

void FUtil::print_hex(FILE *out, const unsigned char *bytes, const int size, const int line_num) {
	int i, j;
	for (i = 0, j = 1; i < size; ++i, ++j) {
		fprintf(out, " %02x", (unsigned) bytes[i]);
		if (j == line_num) {
			fprintf(out, "\n");
			j = 0;
		}
	}
	if (line_num && j != 0) {
		fprintf(out, "\n");
	}
	fflush(out); // ...
}

int FUtil::getErrCode() {
#ifdef __WIN32__
	return ::GetLastError();
#else
	return errno;
#endif
}

string FUtil::getErrStr() {
#ifdef __WIN32__
	const int BUF_SIZE = 1024;
	char buf[BUF_SIZE + 1];
	buf[BUF_SIZE] = '\0';
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, FUtil::getErrCode(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, BUF_SIZE, NULL);
	return buf;
#else
	return ::strerror(errno);
#endif
}

} /* namespace freeyxm */

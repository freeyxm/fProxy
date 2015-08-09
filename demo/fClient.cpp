/*
 * fClient.cpp
 *
 *  Created on: 2012-10-27
 *      Author: freeyxm
 */

#include "fcore/FSocketTcp.h"
#include "fcore/FUtil.h"
#include "fcore/FPmTest.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#ifdef __WIN32__
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace freeyxm;

static pthread_mutex_t g_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_count = 0;
inline void mutexIncCount() {
	pthread_mutex_lock(&g_count_mutex);
	++g_count;
	pthread_mutex_unlock(&g_count_mutex);
}

void conenctServr(void*);

int main(void) {

	setvbuf(stdout, (char*) NULL, _IOLBF, 0);

#if __WIN32__
	static WSADATA wsa_data;
	if (WSAStartup((WORD) (1 << 8 | 1), &wsa_data) != 0) {
		printf("WSAStartup error!\n");
		return -1;
	}
#endif
	printf("client ready!\n");

	ft_funparam_t fp;
	fp.handle = conenctServr;
	fp.param = NULL;

	g_count = 0;
	long t = FPmTest::getRunTime(4, &fp);
	if (t > 0) {
		printf("done! count: %d, time: %ldms, p: %d/s.\n", g_count, t / 1000, (int) (g_count * 1000000.0f / t));
	} else {
		printf("ret = %ld\n", t);
	}

	return 0;
}

void conenctServr(void*) {
	FSocketTcp socket;
	while (g_count < 30000) {
		int ret = socket.connect("127.0.0.1", 2012);
		if (ret) {
			ELOGM_PRINTLN_ERR("connect error!", socket.getErrCode(), socket.getErrStr());
			break;
		} else {
			char buf[1024];
			sprintf(buf, "hello, my socket handle is %d.", socket.getHandle());
			ret = socket.send(buf, strlen(buf));
			//DEBUG_PRINT("send ret: %d\n", ret);

			ret = socket.recv(buf, 1024);
			//DEBUG_PRINT("[%d] recv: %s\n", ret, buf);

			socket.recv(buf, 1024); // to wait server close first ...
			socket.close();
		}
		mutexIncCount();
	} // end while
}

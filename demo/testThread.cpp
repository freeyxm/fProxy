/*
 * test.cpp
 *
 *  Created on: 2012-11-16
 *      Author: kill
 */

#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include "fcore/FUtil.h"
#include "fcore/FString.h"
#include "fcore/FThread.h"

using namespace std;
using namespace freeyxm;

void thread1(void *p);

int main_test_thread(void)
{
	FThread::fun_param param1;
	param1.fun = thread1;
	param1.param = (void*)100;
	FThread t1(&param1);

	FThread::fun_param param2;
	param2.fun = thread1;
	param2.param = (void*)101;
	FThread t2(&param2);

	printf("start t1 ...\n");
	t1.start();
	printf("start t2 ...\n");
	t2.start();

	printf("main ...\n");
	for(int i = 0; i < 50; ++i)
		::usleep(100);

	printf("main cancel t2 ...\n");
	t2.cancel();
	printf("main cancel t2 end.\n");

	t1.join();
	t2.join();
	printf("main end.\n");

	return 0;
}

void thread1(void *p) {
	long tid = (long)::pthread_self();
	long count = (long)p;
	for(int i = 1; i <= count; ++i)
	{
		printf("%ld: %d\n", tid, i);
		::usleep(100);
	}
}

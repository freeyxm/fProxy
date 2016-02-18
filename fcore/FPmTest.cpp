/*
 * FPmTest.cpp
 *
 *  Created on: 2012-11-1
 *      Author: kill
 */

#include "FPmTest.h"
#include <cstdio>
#include <sys/time.h>

namespace freeyxm {

FPmTest::FPmTest() {
}

FPmTest::~FPmTest() {
}

/*
 * testPM: success, return the run time (unit: us); error, return -1.
 */
long FPmTest::getRunTime(const int threadNum, const ft_funparam_t *p_fp) {
	if (!p_fp) {
		fprintf(stdout, "tpm: p_fp not init!\n");
		return -1;
	}

	FThread threads[threadNum];
	for (int i = 0; i < threadNum; ++i) {
		threads[i].setFunParam(p_fp);
	}

	struct timeval st, et;
	gettimeofday(&st, NULL);
	for (int i = 0; i < threadNum; ++i) {
		threads[i].start();
	}
	for (int i = 0; i < threadNum; ++i) {
		threads[i].join();
	}
	gettimeofday(&et, NULL);

	long run_time = (et.tv_sec - st.tv_sec) * 1000000 + (et.tv_usec - st.tv_usec); // us

	return run_time;
}

} /* namespace freeyxm */

/*
 * FPmTest.h
 *
 *  Created on: 2012-11-1
 *      Author: kill
 */

#ifndef FPMTEST_H_
#define FPMTEST_H_

#include "FThread.h"

namespace freeyxm {

class FPmTest {
public:
	FPmTest();
	virtual ~FPmTest();

	static long getRunTime(const int threadNum, const ft_funparam_t *p_fp);
};

} /* namespace freeyxm */
#endif /* FPMTEST_H_ */

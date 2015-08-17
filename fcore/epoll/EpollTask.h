/*
 * EpollTask.h
 *
 *  Created on: 2015年8月14日
 *      Author: aka
 */

#ifndef FCORE_EPOLLTASK_H_
#define FCORE_EPOLLTASK_H_

#include <fcore/FThreadTask.h>

namespace freeyxm {

class SchedulerParam;

class EpollTask: public FThreadTask {
public:
	EpollTask(SchedulerParam *param);
	virtual ~EpollTask();

	void run();

	void* operator new(std::size_t size);
	void operator delete(void *ptr, std::size_t size);
};

} /* namespace freeyxm */

#endif /* FCORE_EPOLLTASK_H_ */

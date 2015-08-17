/*
 * EpollTask.cpp
 *
 *  Created on: 2015年8月14日
 *      Author: aka
 */

#include "EpollTask.h"
#include "EpollScheduler.h"

namespace freeyxm {

EpollTask::EpollTask(SchedulerParam *param) :
		FThreadTask(param)
{
	// TODO Auto-generated constructor stub

}

EpollTask::~EpollTask()
{
	// TODO Auto-generated destructor stub
}

void EpollTask::run()
{
}

void* EpollTask::operator new(std::size_t size)
{
	return FThreadTask::operator new(size);
}

void EpollTask::operator delete(void *ptr, std::size_t size)
{
	FThreadTask::operator delete(ptr, size);
}

} /* namespace freeyxm */

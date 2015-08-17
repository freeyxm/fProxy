/*
 * EpollScheduler.h
 *
 *  Created on: 2015年8月15日
 *      Author: aka
 */

#ifndef FCORE_EPOLL_EPOLLSCHEDULER_H_
#define FCORE_EPOLL_EPOLLSCHEDULER_H_

#include "EpollWatcher.h"
#include <fcore/FThreadPool.h>
#include <list>
#include <map>
#include <pthread.h>
#include <sys/epoll.h>

namespace freeyxm {

typedef void (*SchedulerCallback)();

struct SchedulerParam {
	int sockfd;
	uint32_t events;
	SchedulerCallback rcb;
	SchedulerCallback wcb;
	void *data;
};

class EpollScheduler {
public:
	EpollScheduler(EpollWatcher *watcher);
	virtual ~EpollScheduler();

	void addListenEvent(SchedulerParam *param);

	void addEpollEvent(const struct epoll_event *events, int count);

private:
	EpollWatcher *m_pWatcher;
	FThreadPool m_threadPool;
	std::list<FThreadTask*> m_tasks;
	std::map<int, SchedulerParam*> m_sockInfo;
	pthread_mutex_t m_sockInfoLock;
};

} /* namespace freeyxm */

#endif /* FCORE_EPOLL_EPOLLSCHEDULER_H_ */

/*
 * EpollScheduler.cpp
 *
 *  Created on: 2015年8月15日
 *      Author: aka
 */

#include "EpollScheduler.h"
#include "EpollTask.h"

namespace freeyxm {

EpollScheduler::EpollScheduler(EpollWatcher *watcher) :
		m_pWatcher(watcher), m_threadPool(3, 3)
{
	pthread_mutex_init(&m_sockInfoLock, NULL);
}

EpollScheduler::~EpollScheduler()
{
	// TODO Auto-generated destructor stub
}

void EpollScheduler::addListenEvent(SchedulerParam *param)
{
	pthread_mutex_lock(&m_sockInfoLock);
	m_sockInfo.insert(std::map<int, SchedulerParam*>::value_type(param->sockfd, param));
	pthread_mutex_unlock(&m_sockInfoLock);
}

void EpollScheduler::addEpollEvent(const struct epoll_event *events, int count)
{
	std::map<int, SchedulerParam*>::iterator it;
	pthread_mutex_lock(&m_sockInfoLock);
	for (int i = 0; i < count; ++i)
	{
		int sockfd = events[i].data.fd;
		it = m_sockInfo.find(sockfd);
		if (it != m_sockInfo.end())
		{
			SchedulerParam *param = it->second;
			param->events = events[i].events;
			FThreadTask *task = new EpollTask(param);
			m_tasks.push_back(task);
		}
	}
	pthread_mutex_unlock(&m_sockInfoLock);
	m_threadPool.pushTask(m_tasks);
	m_tasks.clear();
}

} /* namespace freeyxm */

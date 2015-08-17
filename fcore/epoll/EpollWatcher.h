/*
 * EpollWatcher.h
 *
 *  Created on: 2015年8月13日
 *      Author: aka
 */

#ifndef FCORE_EPOLLWATCHER_H_
#define FCORE_EPOLLWATCHER_H_

#include <fcore/FThreadPool.h>
#include <list>
#include <queue>
#include <sys/epoll.h>

namespace freeyxm {

class EpollScheduler;

class EpollWatcher {
public:
	EpollWatcher(int size);
	virtual ~EpollWatcher();

	int addSock(int sockfd, int events);
	int modSock(int sockfd, int events);
	int delSock(int sockfd);

	void setScheduler(EpollScheduler *scheduler);

	int run();

private:
	int init();

private:
	int m_epollfd;
	int m_epollSize;
	struct epoll_event *m_events;
	EpollScheduler *m_pScheduler;
};

} /* namespace freeyxm */

#endif /* FCORE_EPOLLWATCHER_H_ */

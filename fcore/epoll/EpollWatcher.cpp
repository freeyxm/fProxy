/*
 * EpollWatcher.cpp
 *
 *  Created on: 2015年8月13日
 *      Author: aka
 */

#include "EpollWatcher.h"
#include "EpollScheduler.h"
#include <fcore/FLogger.h>
#include <fcore/FUtil.h>
#include <unistd.h>
#include <cstring>

namespace freeyxm {

EpollWatcher::EpollWatcher(int size) :
		m_epollfd(0), m_epollSize(size)
{
	m_events = new epoll_event[m_epollSize];

	init();
}

EpollWatcher::~EpollWatcher()
{
	delete[] m_events;

	if (m_epollfd > 0)
	{
		::close(m_epollfd);
	}
}

int EpollWatcher::init()
{
	m_epollfd = ::epoll_create(m_epollSize);
	if (m_epollfd < 0)
	{
		ELOGM_PRINTLN_ERR("epoll_create error", FUtil::getErrCode(), FUtil::getErrStr());
		return -1;
	}
	return 0;
}

int EpollWatcher::addSock(int sockfd, int events)
{
	struct epoll_event ev;
	::memset(&ev, 0, sizeof(ev));
	ev.data.fd = sockfd;
	ev.events = events;
	return ::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sockfd, &ev);
}

int EpollWatcher::modSock(int sockfd, int events)
{
	struct epoll_event ev;
	::memset(&ev, 0, sizeof(ev));
	ev.data.fd = sockfd;
	ev.events = events;
	return ::epoll_ctl(m_epollfd, EPOLL_CTL_MOD, sockfd, &ev);
}

int EpollWatcher::delSock(int sockfd)
{
	return ::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sockfd, NULL);
}

void EpollWatcher::setScheduler(EpollScheduler *scheduler)
{
	m_pScheduler = scheduler;
}

int EpollWatcher::run()
{
	int ret = 0;
	int nfds = 0;
	while (true)
	{
		nfds = ::epoll_wait(m_epollfd, m_events, m_epollSize, 0);
		if (nfds < 0)
		{
			ELOGM_PRINTLN_ERR("epoll_wait error", FUtil::getErrCode(), FUtil::getErrStr());
			ret = -1;
			break;
		}
		m_pScheduler->addEpollEvent(m_events, nfds);
	}
	return ret;
}

} /* namespace freeyxm */

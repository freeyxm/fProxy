/*
 * FSocketWatcher.cpp
 *
 *  Created on: 2016年2月18日
 *      Author: admin
 */

#include <fcore/FSocketWatcher.h>

FSocketWatcher::FSocketWatcher()
{
	for (int i = 0; i < WAIT_QUEUE_SIZE; ++i)
	{
		m_waitLock[i] = PTHREAD_MUTEX_INITIALIZER;
	}
	for (int i = 0; i < READY_QUEUE_SIZE; ++i)
	{
		m_readyLock[i] = PTHREAD_MUTEX_INITIALIZER;
	}
}

FSocketWatcher::~FSocketWatcher()
{
	for (int i = 0; i < WAIT_QUEUE_SIZE; ++i)
	{
		pthread_mutex_destroy(&m_waitLock[i]);
	}
	for (int i = 0; i < READY_QUEUE_SIZE; ++i)
	{
		pthread_mutex_destroy(&m_readyLock[i]);
	}
}

void FSocketWatcher::pushWait(int index, const SockInfo &sock)
{
	pthread_mutex_lock(&m_waitLock[index]);
	m_waitQueue[index].push(sock);
	pthread_mutex_unlock(&m_waitLock[index]);
}

void FSocketWatcher::popWait()
{
	for (int i = 0; i < WAIT_QUEUE_SIZE; ++i)
	{
		if (!m_waitQueue[i].empty())
		{
			pthread_mutex_lock(&m_waitLock[i]);
			do
			{
				wait(m_waitQueue[i].front());
				m_waitQueue[i].pop();
			} while (!m_waitQueue[i].empty());
			pthread_mutex_unlock(&m_waitLock[i]);
		}
	}
}

void FSocketWatcher::pushReady(const SockInfo &sock)
{
	int index = sock.sock_id % READY_QUEUE_SIZE; // !!!
	pthread_mutex_lock(&m_readyLock[index]);
	m_readyQueue[index].push(sock);
	pthread_mutex_unlock(&m_readyLock[index]);
}

bool FSocketWatcher::popReady(int index, SockInfo &out)
{
	bool bSuccess = false;
	if (!m_readyQueue[index].empty())
	{
		pthread_mutex_lock(&m_readyLock[index]);
		out = m_readyQueue[index].front();
		m_readyQueue[index].pop();
		pthread_mutex_unlock(&m_readyLock[index]);
		bSuccess = true;
	}
	return bSuccess;
}

void FSocketWatcher::wait(const SockInfo &sock)
{
	// TODO wait ...
}

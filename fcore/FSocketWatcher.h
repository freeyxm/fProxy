/*
 * FSocketWatcher.h
 *
 *  Created on: 2016年2月18日
 *      Author: admin
 */

#ifndef FCORE_FSOCKETWATCHER_H_
#define FCORE_FSOCKETWATCHER_H_

#include <queue>
#include <list>
#include <pthread.h>

class FSocketWatcher
{
public:
	static const int READY_QUEUE_SIZE = 2;
	static const int WAIT_QUEUE_SIZE = READY_QUEUE_SIZE + 1;

	FSocketWatcher();
	virtual ~FSocketWatcher();

	enum class SockFlag
	{
		Sock_Read = 1, Sock_Write,
	};

	struct SockInfo
	{
		int sock_id;
		SockFlag flag;
		time_t time;
	};

	void pushWait(int index, const SockInfo &sock);
	void popWait();

	void pushReady(const SockInfo &sock);
	bool popReady(int index, SockInfo &out);

private:
	void wait(const SockInfo &sock);

private:
	std::queue<SockInfo> m_waitQueue[WAIT_QUEUE_SIZE];
	std::queue<SockInfo> m_readyQueue[READY_QUEUE_SIZE];

	pthread_mutex_t m_waitLock[WAIT_QUEUE_SIZE];
	pthread_mutex_t m_readyLock[READY_QUEUE_SIZE];
};

#endif /* FCORE_FSOCKETWATCHER_H_ */

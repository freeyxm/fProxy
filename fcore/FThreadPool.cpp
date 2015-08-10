/*
 * FThreadPool.cpp
 *
 *  Created on: 2015年7月22日
 *      Author: aka
 */

#include "FThreadPool.h"
#include "fcore/FLogger.h"
#include "fcore/FUtil.h"
#include <assert.h>

namespace freeyxm {

FThreadPool::FThreadPool(size_t size, size_t maxSize) :
		m_size(0), m_maxSize(maxSize), m_taskQueueSize(0), m_running(false)
{
	init();
	createTheads(size);
}

FThreadPool::~FThreadPool()
{
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
	pthread_attr_destroy(&m_attr);
}

int FThreadPool::init()
{
	m_mutex = PTHREAD_MUTEX_INITIALIZER;
	m_cond = PTHREAD_COND_INITIALIZER;

	pthread_attr_init(&m_attr);
	// set m_attr ...

	m_running = true;

	return 0;
}

int FThreadPool::createThead()
{
	pthread_t tid;
	int ret = pthread_create(&tid, &m_attr, ThreadRun, this);
	if (ret == 0)
	{
		m_idle.insert(tid);
		++m_size;
		DLOGM_PRINT_T("create thread %lu, current size = %lu\n", (long unsigned int)tid, m_size);
	}
	else
	{
		ELOGM_PRINTLN_ERR("create thread error", ret, FUtil::getErrStr(ret));
	}
	return ret;
}

int FThreadPool::cancelThread(pthread_t tid)
{
	int ret = pthread_cancel(tid);
	if (ret == 0)
	{
		m_idle.erase(tid);
		m_busy.erase(tid);
		--m_size;
		DLOGM_PRINT_T("cancel thread %lu, current size = %lu\n", (long unsigned int)tid, m_size);
	}
	else
	{
		ELOGM_PRINTLN_ERR("cancel thread error", ret, FUtil::getErrStr(ret));
	}
	return ret;
}

int FThreadPool::createTheads(int num)
{
	int count = 0;
	pthread_mutex_lock(&m_mutex);
	for (int i = 0; i < num; ++i)
	{
		if (createThead() == 0)
			++count;
	}
	pthread_mutex_unlock(&m_mutex);
	return count;
}

void* FThreadPool::ThreadRun(void *arg)
{
	FThreadPool *pool = (FThreadPool*) arg;
	pthread_t tid = pthread_self();

	while (true)
	{
		pthread_mutex_lock(&pool->m_mutex);
		if (!pool->hasTask())
		{
			pool->moveToIdle(tid);
			pthread_cond_wait(&pool->m_cond, &pool->m_mutex);
			pool->moveToBusy(tid);
		}
		else
		{
			pool->moveToBusy(tid);
		}
		FThreadTask *task = pool->popTask();
		pthread_mutex_unlock(&pool->m_mutex);

		if (task != NULL)
		{
			task->run();
			delete task;
		}
	}

	return NULL;
}

inline void FThreadPool::moveToIdle(pthread_t tid)
{
	if (isOverflow())
	{
		cancelThread(tid);
	}
	else
	{
		if (m_idle.count(tid) == 0)
		{
			m_busy.erase(tid);
			m_idle.insert(tid);
		}
	}
}

inline void FThreadPool::moveToBusy(pthread_t tid)
{
	if (m_busy.count(tid) == 0)
	{
		m_idle.erase(tid);
		m_busy.insert(tid);
	}
}

inline bool FThreadPool::isBusy(pthread_t tid)
{
	return m_busy.count(tid) > 0;
}

inline bool FThreadPool::isFull()
{
	return m_maxSize > 0 && m_size >= m_maxSize;
}

inline bool FThreadPool::isOverflow()
{
	return m_maxSize > 0 && m_size > m_maxSize;
}

bool FThreadPool::pushTask(FThreadTask *task)
{
	bool ret = false;
	pthread_mutex_lock(&m_mutex);
	if (!isTaskFull())
	{
		m_tasks.push_back(task);
		if (m_idle.empty() && !isFull())
		{
			createThead();
		}
		ret = true;
	}
	pthread_mutex_unlock(&m_mutex);
	pthread_cond_signal(&m_cond);
	return ret;
}

inline FThreadTask* FThreadPool::popTask()
{
	if (!m_tasks.empty())
	{
		FThreadTask *task = m_tasks.front();
		m_tasks.pop_front();
		return task;
	}
	else
	{
		return NULL;
	}
}

inline bool FThreadPool::hasTask()
{
	return !m_tasks.empty();
}

inline bool FThreadPool::isTaskFull()
{
	return m_taskQueueSize > 0 && m_tasks.size() >= m_taskQueueSize;
}

void FThreadPool::setMaxSize(size_t maxSize)
{
	pthread_mutex_lock(&m_mutex);
	m_maxSize = maxSize;
	if (m_maxSize > 0)
	{
		while (m_size > m_maxSize)
		{
			if (m_idle.empty())
				break;
			std::set<pthread_t>::iterator it = m_idle.begin();
			pthread_t tid = *it;
			if (cancelThread(tid) != 0)
				break;
		}
	}
	pthread_mutex_unlock(&m_mutex);
}

void FThreadPool::setTaskQueueSize(size_t maxSize)
{
	m_taskQueueSize = maxSize;
}

void FThreadPool::printStatus()
{
	pthread_mutex_lock(&m_mutex);
	DLOGM_PRINT_T("[pool] maxSize: %lu, size: %lu, busy: %lu, idle: %lu, task: %lu\n", m_maxSize, m_size, m_busy.size(),
			m_idle.size(), m_tasks.size());
	pthread_mutex_unlock(&m_mutex);
}

} /* namespace freeyxm */

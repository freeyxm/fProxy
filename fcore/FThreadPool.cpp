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
		m_size(0), m_maxSize(maxSize), m_taskMaxSize(0), m_running(false)
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
	int ret = pthread_create(&tid, &m_attr, StartThread, this);
	if (ret == 0)
	{
		m_idle.insert(tid);
		++m_size;
		DEBUG_PRINT_T("create thread %lu, current size = %lu\n", tid, m_size);
	}
	else
	{
		LOG_PRINTLN_ERR("create thread error", ret, FUtil::getErrStr().c_str());
	}
	return ret;
}

void FThreadPool::createTheads(int num)
{
	pthread_mutex_lock(&m_mutex);
	for (int i = 0; i < num; ++i)
	{
		createThead();
	}
	pthread_mutex_unlock(&m_mutex);
}

void* FThreadPool::StartThread(void *arg)
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
		else if (!pool->isBusy(tid))
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

void FThreadPool::moveToIdle(pthread_t tid)
{
	m_busy.erase(tid);
	m_idle.insert(tid);
}

void FThreadPool::moveToBusy(pthread_t tid)
{
	m_idle.erase(tid);
	m_busy.insert(tid);
}

bool FThreadPool::isBusy(pthread_t tid)
{
	return m_busy.count(tid) > 0;
}

void FThreadPool::pushTask(FThreadTask *task)
{
	pthread_mutex_lock(&m_mutex);
	m_tasks.push_back(task);
	if (m_idle.size() == 0 && m_size < m_maxSize)
	{
		createThead();
	}
	pthread_mutex_unlock(&m_mutex);
	pthread_cond_signal(&m_cond);
}

FThreadTask* FThreadPool::popTask()
{
	if (m_tasks.size() > 0)
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

bool FThreadPool::hasTask()
{
	return m_tasks.size() > 0;
}

void FThreadPool::printStatus()
{
	LOG_PRINT_T("maxSize: %lu, size: %lu, busy: %lu, idle: %lu, task: %lu\n", m_maxSize, m_size, m_busy.size(), m_idle.size(),
			m_tasks.size());
}

} /* namespace freeyxm */

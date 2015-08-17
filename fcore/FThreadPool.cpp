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

FThreadPool::FThreadPool(size_t size, size_t holeNum) :
		m_size(0), m_taskQueueNum(holeNum), m_taskQueueSize(0), m_running(false)
{
	m_tasks = new std::list<FThreadTask*>[m_taskQueueNum];
	m_tasksTemp = new std::list<FThreadTask*>[m_taskQueueNum];
	m_taskLocks = new pthread_mutex_t[m_taskQueueNum];
	m_taskConds = new pthread_cond_t[m_taskQueueNum];
	m_taskCount.store(0);

	init();
	createTheads(size);
}

FThreadPool::~FThreadPool()
{
	pthread_mutex_destroy(&m_mutex);
	pthread_attr_destroy(&m_attr);

	for (size_t i = 0; i < m_taskQueueNum; ++i)
	{
		std::list<FThreadTask*>::iterator it = m_tasks[i].begin();
		while (it != m_tasks[i].end())
		{
			delete *it;
		}
		m_tasks[i].clear();

		it = m_tasksTemp[i].begin();
		while (it != m_tasksTemp[i].end())
		{
			delete *it;
		}
		m_tasksTemp[i].clear();

		pthread_mutex_destroy(&m_taskLocks[i]);
		pthread_cond_destroy(&m_taskConds[i]);
	}
	delete[] m_tasks;
	delete[] m_tasksTemp;
	delete[] m_taskLocks;
	delete[] m_taskConds;
}

int FThreadPool::init()
{
	pthread_mutex_init(&m_mutex, NULL);

	for (size_t i = 0; i < m_taskQueueNum; ++i)
	{
		pthread_mutex_init(&m_taskLocks[i], NULL);
		pthread_cond_init(&m_taskConds[i], NULL);
	}

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
		m_threadIndexMap.insert(std::map<pthread_t, size_t>::value_type(tid, m_size));
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
		m_threadIndexMap.erase(tid);
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
	int index = pool->hashThread(tid);

	while (true)
	{
		pthread_mutex_lock(&pool->m_mutex);
		if (!pool->hasTask(tid))
		{
			pool->moveToIdle(tid);
			pthread_cond_wait(&pool->m_taskConds[index], &pool->m_mutex);
			pool->moveToBusy(tid);
		}
		else
		{
			pool->moveToBusy(tid);
		}
		pthread_mutex_unlock(&pool->m_mutex);

		FThreadTask *task = pool->popTask();
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
	if (m_idle.count(tid) == 0)
	{
		m_busy.erase(tid);
		m_idle.insert(tid);
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

int FThreadPool::pushTask(FThreadTask *task)
{
	if (isTaskFull())
		return 0;

	int index = hashTask(task);
	pthread_mutex_lock(&m_taskLocks[index]);
	{
		m_tasks[index].push_back(task);
	}
	pthread_mutex_unlock(&m_taskLocks[index]);

	m_taskCount.fetch_add(1);

	pthread_cond_signal(&m_taskConds[index]);

	return 1;
}

int FThreadPool::pushTask(const std::list<FThreadTask*> &tasks)
{
	int count = 0;
	std::list<FThreadTask*>::const_iterator it = tasks.begin();
	while (it != tasks.end())
	{
		FThreadTask *task = *it;
		int index = hashTask(task);
		m_tasksTemp[index].push_back(task);
	}

	for (size_t i = 0; i < m_taskQueueNum; ++i)
	{
		std::list<FThreadTask*> &taskList = m_tasksTemp[i];
		if (taskList.empty())
			continue;

		int num = 0;
		it = taskList.begin();
		pthread_mutex_lock(&m_taskLocks[i]);
		while (!isTaskFull() && it != taskList.end())
		{
			FThreadTask *task = *it;
			int index = hashTask(task);
			m_tasks[index].push_back(task);
			++num;
			++it;
		}
		pthread_mutex_unlock(&m_taskLocks[i]);

		if (num > 0)
		{
			m_taskCount.fetch_add(num);
			count += num;
		}

		for (int i = 0; i < num; ++i)
		{
			pthread_cond_signal(&m_taskConds[i]);
		}

		taskList.clear();
	}

	return count;
}

inline FThreadTask* FThreadPool::popTask()
{
	FThreadTask *task = NULL;
	int index = hashThread(pthread_self());

	pthread_mutex_lock(&m_taskLocks[index]);
	if (!m_tasks[index].empty())
	{
		task = m_tasks[index].front();
		m_tasks[index].pop_front();
		m_taskCount.fetch_sub(1);
	}
	pthread_mutex_unlock(&m_taskLocks[index]);

	return task;
}

inline int FThreadPool::hashTask(FThreadTask *task)
{
	return ((size_t) task) % m_taskQueueNum;
}

inline int FThreadPool::hashThread(pthread_t tid)
{
	std::map<pthread_t, size_t>::iterator it = m_threadIndexMap.find(tid);
	if (it != m_threadIndexMap.end())
	{
		return it->second % m_taskQueueNum;
	}
	else
	{
		return ((size_t) tid) % m_taskQueueNum;
	}
}

inline bool FThreadPool::hasTask(pthread_t tid)
{
	int index = hashThread(tid);
	return !m_tasks[index].empty();
}

inline bool FThreadPool::isTaskFull()
{
	return m_taskQueueSize > 0 && (size_t) m_taskCount >= m_taskQueueSize;
}

void FThreadPool::setTaskQueueSize(size_t maxSize)
{
	m_taskQueueSize = maxSize;
}

void FThreadPool::printStatus()
{
	pthread_mutex_lock(&m_mutex);
	DLOGM_PRINT_T("[pool] size: %3lu, busy: %3lu, idle: %3lu, task: %d\n", m_size, m_busy.size(), m_idle.size(),
			m_taskCount.load());
	pthread_mutex_unlock(&m_mutex);
}

} /* namespace freeyxm */

/*
 * FThreadPool.h
 *
 *  Created on: 2015年7月22日
 *      Author: aka
 */

#ifndef FCORE_FTHREADPOOL_H_
#define FCORE_FTHREADPOOL_H_

#include "FThreadTask.h"
#include <pthread.h>
#include <set>
#include <list>

namespace freeyxm {

class FThreadPool {
public:
	FThreadPool(size_t size, size_t maxSize);
	virtual ~FThreadPool();

	bool pushTask(FThreadTask *task);

	void setMaxSize(size_t maxSize);
	void setTaskQueueSize(size_t maxSize);

	void printStatus();

private:
	int init();

	int createThead();
	int cancelThread(pthread_t tid);
	int createTheads(int num);

	inline void moveToIdle(pthread_t tid);
	inline void moveToBusy(pthread_t tid);
	inline bool isBusy(pthread_t tid);

	inline bool isFull();
	inline bool isOverflow();

	inline FThreadTask* popTask();
	inline bool hasTask();
	inline bool isTaskFull();

	static void* StartThread(void *arg);

private:
	size_t m_size;
	size_t m_maxSize;
	std::set<pthread_t> m_idle;
	std::set<pthread_t> m_busy;

	size_t m_taskQueueSize;
	std::list<FThreadTask*> m_tasks;

	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	pthread_attr_t m_attr;

	bool m_running;
};

} /* namespace freeyxm */

#endif /* FCORE_FTHREADPOOL_H_ */

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

	void pushTask(FThreadTask *task);

	void printStatus();

private:
	int init();

	int createThead();
	void createTheads(int num);

	void moveToIdle(pthread_t tid);
	void moveToBusy(pthread_t tid);
	bool isBusy(pthread_t tid);

	FThreadTask* popTask();
	bool hasTask();

	static void* StartThread(void *arg);

private:
	size_t m_size;
	size_t m_maxSize;
	std::set<pthread_t> m_idle;
	std::set<pthread_t> m_busy;

	int m_taskMaxSize;
	std::list<FThreadTask*> m_tasks;

	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	pthread_attr_t m_attr;

	bool m_running;
};

} /* namespace freeyxm */

#endif /* FCORE_FTHREADPOOL_H_ */

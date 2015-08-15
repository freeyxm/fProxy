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
#include <atomic>
#include <map>

namespace freeyxm {

class FThreadPool {
public:
	FThreadPool(size_t size, size_t holeNum = 1);
	virtual ~FThreadPool();

	int pushTask(FThreadTask *task);
	int pushTask(const std::list<FThreadTask*> &tasks);

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

	inline FThreadTask* popTask();
	inline bool hasTask(pthread_t tid);
	inline bool isTaskFull();
	inline int hashTask(FThreadTask *task);
	inline int hashThread(pthread_t tid);

	static void* ThreadRun(void *arg);

private:
	size_t m_size;
	std::set<pthread_t> m_idle;
	std::set<pthread_t> m_busy;

	size_t m_taskQueueNum;
	size_t m_taskQueueSize;
	std::list<FThreadTask*> *m_tasks;
	std::list<FThreadTask*> *m_tasksTemp;
	pthread_mutex_t *m_taskLocks;
	pthread_cond_t *m_taskConds;
	std::atomic<int> m_taskCount;

	pthread_mutex_t m_mutex;
	pthread_attr_t m_attr;

	std::map<pthread_t, size_t> m_threadIndexMap;

	bool m_running;
};

} /* namespace freeyxm */

#endif /* FCORE_FTHREADPOOL_H_ */

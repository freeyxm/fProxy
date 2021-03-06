/*
 * PtrMgr.h
 *
 *  Created on: 2015年7月21日
 *      Author: aka
 */

#ifndef FCORE_FPTRMGR_H_
#define FCORE_FPTRMGR_H_

#include <pthread.h>
#include <set>

namespace freeyxm {

template<typename T>
class FPtrMgr {
public:
	FPtrMgr(size_t maxSize, bool isArray = false);
	virtual ~FPtrMgr();

	T* apply();
	void release(T *ptr);

	bool addFree(T *ptr);
	bool addBusy(T *ptr);

	virtual void clear(T *ptr);
	void clear();

	bool isFull();

	void printStatus(const char *tag);

private:
	size_t m_maxSize;
	bool m_isArray;
	std::set<T*> m_free;
	std::set<T*> m_busy;
	pthread_mutex_t m_mutex;
};

} /* namespace freeyxm */

#endif /* FCORE_FPTRMGR_H_ */

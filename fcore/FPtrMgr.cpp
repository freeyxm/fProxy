/*
 * PtrMgr.cpp
 *
 *  Created on: 2015年7月21日
 *      Author: aka
 */

#include <fcore/FPtrMgr.h>
#include <fcore/FLogger.h>
#include <cstdlib>

namespace freeyxm {

template<typename T>
FPtrMgr<T>::FPtrMgr(size_t maxSize, bool isArray) :
		m_maxSize(maxSize), m_isArray(isArray)
{
	m_lock = PTHREAD_MUTEX_INITIALIZER;
}

template<typename T>
FPtrMgr<T>::~FPtrMgr()
{
	clear();
	pthread_mutex_destroy(&m_lock);
}

template<typename T>
T* FPtrMgr<T>::apply()
{
	T *ptr = NULL;
	pthread_mutex_lock(&m_lock);
	if (m_free.size() > 0)
	{
		ptr = *m_free.begin();
	}
	pthread_mutex_unlock(&m_lock);
	return ptr;
}

template<typename T>
void FPtrMgr<T>::release(T* ptr)
{
	pthread_mutex_lock(&m_lock);
	typename std::set<T*>::iterator it = m_busy.find(ptr);
	if (it != m_busy.end())
	{
		m_free.insert(ptr);
		m_busy.erase(it);
	}
	pthread_mutex_unlock(&m_lock);
}

template<typename T>
bool FPtrMgr<T>::addFree(T* ptr)
{
	bool ret = false;
	pthread_mutex_lock(&m_lock);
	if (!isFull())
	{
		m_free.insert(ptr);
		ret = true;
	}
	pthread_mutex_unlock(&m_lock);
	return ret;
}

template<typename T>
bool FPtrMgr<T>::addBusy(T* ptr)
{
	bool ret = false;
	pthread_mutex_lock(&m_lock);
	if (!isFull())
	{
		m_busy.insert(ptr);
		ret = true;
	}
	pthread_mutex_unlock(&m_lock);
	return ret;
}

template<typename T>
void FPtrMgr<T>::clear(T *ptr)
{
	if (m_isArray)
		delete[] ptr;
	else
		delete ptr;
}

template<typename T>
void FPtrMgr<T>::clear()
{
	for (typename std::set<T*>::iterator it = m_free.begin(); it != m_free.end(); ++it)
	{
		clear(*it);
	}
	m_free.clear();
	for (typename std::set<T*>::iterator it = m_busy.begin(); it != m_busy.end(); ++it)
	{
		clear(*it);
	}
	m_busy.clear();
}

template<typename T>
bool FPtrMgr<T>::isFull()
{
	return m_maxSize > 0 && m_free.size() + m_busy.size() >= m_maxSize;
}

template<typename T>
void FPtrMgr<T>::printStatus(const char *tag)
{
	int free = m_free.size();
	int busy = m_busy.size();
	LOG_PRINT_T("[%s] maxSize: %lu, size: %d, busy: %d, free: %d\n", tag, m_maxSize, busy + free, busy, free);
}

} /* namespace freeyxm */

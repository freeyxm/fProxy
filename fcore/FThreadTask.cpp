/*
 * FThreadTask.cpp
 *
 *  Created on: 2015年7月22日
 *      Author: aka
 */

#include "FThreadTask.h"
#include "FPtrMgr.cpp"
#include <cstdlib>

namespace freeyxm {

#if defined(_DEBUG_) && defined(LOG_LEVEL) && LOG_LEVEL <= LOG_LEVEL_DEBUG
#define PRINT_PTR_MGR_STATUS(tag) m_ptrMgr.printStatus(tag)
#else
#define PRINT_PTR_MGR_STATUS(tag) ;
#endif

FPtrMgr<FThreadTask> FThreadTask::m_ptrMgr = FPtrMgr<FThreadTask>(0);

FThreadTask::FThreadTask(void *data) :
		m_data(data)
{
}

FThreadTask::~FThreadTask()
{
}

void FThreadTask::run()
{
}

void FThreadTask::setData(void *data)
{
	m_data = data;
}

void* FThreadTask::operator new(std::size_t size)
{
	void *ptr = m_ptrMgr.apply();
	if (!ptr)
	{
		ptr = ::malloc(size);
		if (!ptr)
		{
			ELOGM_PRINTLN_FL("malloc failed");
		}
		else
		{
			m_ptrMgr.addBusy(static_cast<FThreadTask*>(ptr));
			PRINT_PTR_MGR_STATUS("_new");
		}
	}
	return ptr;
}

void FThreadTask::operator delete(void *ptr, std::size_t size)
{
	m_ptrMgr.release(static_cast<FThreadTask*>(ptr));
	PRINT_PTR_MGR_STATUS("_del");
}

} /* namespace freeyxm */

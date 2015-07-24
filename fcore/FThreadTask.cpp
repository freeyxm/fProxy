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

#if _DEBUG_
#define PRINT_PTR_MGR_STATUS(tag) m_ptrMgr.printStatus(tag)
#else
#define PRINT_PTR_MGR_STATUS(tag) ;
#endif

FPtrMgr<FThreadTask> FThreadTask::m_ptrMgr = FPtrMgr<FThreadTask>(0);

FThreadTask::FThreadTask(void *data) :
		m_data(data)
{
	// TODO Auto-generated constructor stub
}

FThreadTask::~FThreadTask()
{
	// TODO Auto-generated destructor stub
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
			DEBUG_PRINTLN_FL("malloc failed");
		}
		else
		{
			m_ptrMgr.addBusy(static_cast<FThreadTask*>(ptr));
			PRINT_PTR_MGR_STATUS("new");
		}
	}
	return ptr;
}

void FThreadTask::operator delete(void *ptr, std::size_t size)
{
	m_ptrMgr.release(static_cast<FThreadTask*>(ptr));
	PRINT_PTR_MGR_STATUS("del");
}

} /* namespace freeyxm */

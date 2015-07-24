/*
 * FServerTask.cpp
 *
 *  Created on: 2015年7月23日
 *      Author: aka
 */

#include "FServerTask.h"

namespace freeyxm {

FServerTask::FServerTask(FServer *pServer, FSocketTcp *pSocket) :
		FThreadTask(pSocket), m_pServer(pServer)
{
	// TODO Auto-generated constructor stub
}

FServerTask::~FServerTask()
{
	// TODO Auto-generated destructor stub
}

void FServerTask::run()
{
	FSocketTcp *pSocket = static_cast<FSocketTcp*>(m_data);
	fs_fun_t processFun = m_pServer->getProcessFun();
	if (processFun && pSocket)
	{
		processFun(pSocket);
	}
	m_pServer->taskDone(pSocket);
}

void* FServerTask::operator new(std::size_t size)
{
	return FThreadTask::operator new(size);
}

void FServerTask::operator delete(void *ptr, std::size_t size)
{
	FThreadTask::operator delete(ptr, size);
}

} /* namespace freeyxm */

/*
 * FServerTask.h
 *
 *  Created on: 2015年7月23日
 *      Author: aka
 */

#ifndef UTIL_FSERVERTASK_H_
#define UTIL_FSERVERTASK_H_

#include <fcore/FThreadTask.h>
#include "FServer.h"

namespace freeyxm {

class FServerTask: public FThreadTask {
public:
	FServerTask(FServer *pServer, FSocketTcp *pSocket);
	virtual ~FServerTask();

	void run();

	void* operator new(std::size_t size);
	void operator delete(void *ptr, std::size_t size);

private:
	FServer *m_pServer;
};

} /* namespace freeyxm */

#endif /* UTIL_FSERVERTASK_H_ */

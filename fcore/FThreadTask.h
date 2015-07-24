/*
 * FThreadTask.h
 *
 *  Created on: 2015年7月22日
 *      Author: aka
 */

#ifndef FCORE_FTHREADTASK_H_
#define FCORE_FTHREADTASK_H_

#include "fcore/FPtrMgr.h"
#include <memory>

namespace freeyxm {

class FThreadTask {
public:
	FThreadTask(void *data = NULL);
	virtual ~FThreadTask();

	virtual void run();

	void setData(void *data);

	void* operator new(std::size_t size);
	void operator delete(void *ptr, std::size_t size);

protected:
	void *m_data;
	static FPtrMgr<FThreadTask> m_ptrMgr;
};

} /* namespace freeyxm */

#endif /* FCORE_FTHREADTASK_H_ */

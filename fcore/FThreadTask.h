/*
 * FThreadTask.h
 *
 *  Created on: 2015年7月22日
 *      Author: aka
 */

#ifndef FCORE_FTHREADTASK_H_
#define FCORE_FTHREADTASK_H_

namespace freeyxm {

class FThreadTask {
public:
	FThreadTask(void *data);
	virtual ~FThreadTask();

	virtual void run();

protected:
	void *m_data;
};

} /* namespace freeyxm */

#endif /* FCORE_FTHREADTASK_H_ */

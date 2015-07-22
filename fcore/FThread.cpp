/*
 * FThread.cpp
 *
 *  Created on: 2012-10-26
 *      Author: kill
 */

#include "FThread.h"
#include "fcore/FUtil.h"
#include <cstring>
#include <cstdlib>
#if __linux__
#include <errno.h>
#endif

namespace freeyxm {

FThread::FThread(const ft_funparam_t *p_fp) {
	this->threadHandle = 0; // ...
	if (p_fp) {
		::memcpy(&this->funparam, p_fp, sizeof(this->funparam));
	}
}

FThread::~FThread() {
}

int FThread::start() {
	int ret = testThread();
	if (ret) {
		return ret;
	}
	void *p_fp = ::malloc(sizeof(this->funparam)); // free by startThread if pthread_create call success !!!
	::memcpy(p_fp, &this->funparam, sizeof(this->funparam));
	ret = ::pthread_create(&this->threadHandle, NULL, startThread, p_fp);
	if (ret) {
		this->threadHandle = 0;
		::free(p_fp);
		return ret;
	}
	return 0;
}

void* FThread::startThread(void *p_param) {
	ft_funparam_t *p_fp = (ft_funparam_t*) p_param;
	//DEBUG_PRINT_T("thread %d working, handle: %d, param: %d.\n", ::pthread_self(), p_fp->handle, p_fp->param);
	(*(p_fp->handle))(p_fp->param);
	::free(p_param); // !!!
	//DEBUG_PRINT_T("thread %d done!\n", ::pthread_self());
	return NULL;
}

int FThread::testThread() {
	int ret = 0;
	if (this->threadHandle) {
		// test thread whether running or not ...
		int kill_rc = ::pthread_kill(this->threadHandle, 0);
		switch (kill_rc) {
		case 0: // Success, a thread already running.
			ret = 1;
			break;
		case ESRCH: // No such process.
			::pthread_join(this->threadHandle, NULL); // ...
			this->threadHandle = 0;
			ret = 0;
			break;
		default: // Error occured.
			ret = -1;
			break;
		}
	}
	return ret;
}

int FThread::join() {
	if (this->threadHandle) {
		int ret = FThread::join(this->threadHandle);
		this->threadHandle = 0;
		return ret;
	}
	return 0;
}

int FThread::detach() {
	if (this->threadHandle) {
		return FThread::detach(this->threadHandle);
	}
	return 0;
}

int FThread::cancel() {
	if (this->threadHandle) {
		return FThread::cancel(this->threadHandle);
	}
	return 0;
}

int FThread::kill(const int signum) {
	if (this->threadHandle) {
		return FThread::kill(this->threadHandle, signum);
	}
	return 0;
}

int FThread::setFunParam(const ft_funparam_t *p_fp) {
	int ret = testThread();
	if (ret) {
		return ret;
	}
	::memcpy(&this->funparam, p_fp, sizeof(this->funparam));
	return 0;
}

pthread_t FThread::self() {
	return ::pthread_self();
}

int FThread::join(pthread_t tid) {
	return ::pthread_join(tid, NULL);
}

int FThread::detach(pthread_t tid) {
	return ::pthread_detach(tid);
}

int FThread::cancel(pthread_t tid) {
	return ::pthread_cancel(tid);
}

int FThread::kill(pthread_t tid, int signum) {
	return ::pthread_kill(tid, signum);
}

} /* namespace freeyxm */

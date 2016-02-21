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
#ifdef __linux__
#include <errno.h>
#endif

namespace freeyxm {

FThread::FThread(const fun_param *fp) {
	clear();
	if (fp) {
		this->m_funp = *fp;
	}
}

FThread::~FThread() {
}

void FThread::clear() {
	m_handle = 0;
	m_running = false;
}

int FThread::start() {
	int ret = stop();
	if (ret) {
		return ret;
	}

	m_running = true;
	ret = ::pthread_create(&this->m_handle, NULL, startThread, this);
	if (ret) {
		clear();
		return ret;
	}
	return 0;
}

void* FThread::startThread(void *param) {
	FThread *thread = (FThread*) param;
	//DLOG_PRINT_T("thread %d start ...\n", ::pthread_self());
	(*(thread->m_funp.fun))(thread->m_funp.param);
	thread->m_running = false;
	//DLOG_PRINT_T("thread %d done!\n", ::pthread_self());
	return NULL;
}

int FThread::stop() {
	int ret = 0;
	if (this->m_handle) {
		// test thread whether running or not ...
		int kill_rc = ::pthread_kill(this->m_handle, 0);
		switch (kill_rc) {
		case 0: // Success, a thread already running.
			ret = 1;
			break;
		case ESRCH: // No such process.
			join();
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
	if (this->m_handle) {
		int ret = FThread::join(this->m_handle);
		clear();
		return ret;
	}
	return 0;
}

int FThread::detach() {
	if (this->m_handle) {
		return FThread::detach(this->m_handle);
	}
	return 0;
}

int FThread::cancel() {
	if (this->m_handle) {
		return FThread::cancel(this->m_handle);
	}
	return 0;
}

int FThread::kill(const int signum) {
	if (this->m_handle) {
		return FThread::kill(this->m_handle, signum);
	}
	return 0;
}

int FThread::setFunParam(const fun_param *p_fp) {
	this->m_funp = *p_fp;
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

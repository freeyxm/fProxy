/*
 * FThread.h
 *
 *  Created on: 2012-10-26
 *      Author: freeyxm
 */

#ifndef FTHREAD_H_
#define FTHREAD_H_

#include <pthread.h>
#include <signal.h>

namespace freeyxm {

#ifndef SIGALRM
#define SIGALRM 14
#endif
#ifndef SIGUSR1
#define SIGUSR1 10
#endif

class FThread {
public:
	typedef void (*thread_fun)(void*);

	typedef struct {
		thread_fun fun; // function call by new thread.
		void *param; // pointer to param deliver to function call by new thread.
	} fun_param;

	FThread(const fun_param *fp = NULL);
	virtual ~FThread();

	int start();
	int join();
	int detach();
	int cancel();
	int kill(const int signum);

	int setFunParam(const fun_param *fp);

	static pthread_t self();
	static int join(pthread_t tid);
	static int detach(pthread_t tid);
	static int cancel(pthread_t tid);
	static int kill(pthread_t tid, int signum);

private:
	static void* startThread(void*);
	int stop();
	void clear();

private:
	pthread_t m_handle;
	fun_param m_funp;
	bool m_running;
};

} /* namespace freeyxm */
#endif /* FTHREAD_H_ */

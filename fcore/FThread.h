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

typedef void (*ft_fun_t)(void*);

typedef struct {
	ft_fun_t handle; // function call by new thread.
	void *param; // pointer to param deliver to function call by new thread.
} ft_funparam_t;

class FThread {
public:
	FThread(const ft_funparam_t *p_fp = NULL);
	virtual ~FThread();

	int start();
	int join();
	int detach();
	int cancel();
	int kill(const int signum);

	int setFunParam(const ft_funparam_t *p_fp);

	static pthread_t self();
	static int join(pthread_t tid);
	static int detach(pthread_t tid);
	static int cancel(pthread_t tid);
	static int kill(pthread_t tid, int signum);

private:
	static void* startThread(void*);
	int testThread();

private:
	pthread_t threadHandle;
	ft_funparam_t funparam;
};

} /* namespace freeyxm */
#endif /* FTHREAD_H_ */

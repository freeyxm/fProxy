/*
 * FServer.h
 *
 *  Created on: 2012-10-24
 *      Author: freeyxm
 */

#ifndef FSERVER_H_
#define FSERVER_H_

#include "FSocketTcp.h"
#include <string>
#include <semaphore.h>

namespace freeyxm {

using std::string;

typedef void (*fs_fun_t)(FSocketTcp*);

class FServer {
public:
	FServer(const string addr = "", const int port = 2012,
			const unsigned int max_conn_num = 0);
	virtual ~FServer();

	void setProcessFun(const fs_fun_t fun);
	int run();

	void setListenQueueLen(int queue_len);
	int getListenQueueLen();
	int getMaxConnNum();

protected:
	virtual int loop();

private:
	int init();
	int setSignal();
	static void sigHandler(int signum);
	int process(FSocketTcp *socket);
	static void thread_proxy(void*);

private:
	FSocketTcp server_socket;
	fs_fun_t funHandle; // the function call by every connection.
	string addr;
	int port;
	int stopped;
	int listen_queue_len;
	const int max_conn_num;
	sem_t *p_max_conn_num_sem;
};

} /* namespace freeyxm */
#endif /* FSERVER_H_ */

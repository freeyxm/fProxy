/*
 * FServer.h
 *
 *  Created on: 2012-10-24
 *      Author: freeyxm
 */

#ifndef FSERVER_H_
#define FSERVER_H_

#include "fcore/FSocketTcp.h"
#include "fcore/FThreadPool.h"
#include <string>
#include <semaphore.h>

namespace freeyxm {

using std::string;

typedef void (*fs_fun_t)(FSocketTcp*);

class FServer {
public:
	FServer(const string addr = "", const int port = 2012, const unsigned int max_conn_num = 0);
	virtual ~FServer();

	int run();

	void setProcessFun(const fs_fun_t fun);
	fs_fun_t getProcessFun();

	void setListenQueueLen(int queue_len);
	int getListenQueueLen();
	int getMaxConnNum();

	void taskDone(FSocketTcp *socket);

protected:
	virtual int loop();

private:
	int init();
	int setSignal();
	static void sigHandler(int signum);

private:
	FSocketTcp m_serverSocket;
	FThreadPool m_threadPool;
	fs_fun_t m_funHandle; // the function call by every connection.
	string m_addr;
	int m_port;
	int m_listenQueueLen;
	int m_maxConnNum;
	sem_t m_maxConnNumSem;
	bool m_running;
	bool m_inited;
};

} /* namespace freeyxm */
#endif /* FSERVER_H_ */

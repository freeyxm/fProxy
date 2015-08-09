/*
 * FServer.cpp
 *
 *  Created on: 2012-10-24
 *      Author: kill
 */

#include "FServer.h"
#include "FServerTask.h"
#include "fcore/FUtil.h"
#include "fcore/FThread.h"
#include <signal.h>
#include <cstdlib>
#if __linux__
#include <errno.h>
#include <unistd.h>
#endif

namespace freeyxm {

static FServer *g_server;

FServer::FServer(const string addr, const int port, const unsigned int max_conn_num) :
		m_serverSocket(), m_threadPool(10, 200), m_addr(addr), m_port(port), m_maxConnNum(max_conn_num)
{
	m_funHandle = NULL;
	m_listenQueueLen = 10;
	m_running = false;
	m_inited = false;
	g_server = this;
}

FServer::~FServer()
{
	if (m_inited)
	{
		sem_destroy(&m_maxConnNumSem);
	}
	m_serverSocket.close();
}

int FServer::init()
{
	if (!this->m_funHandle)
	{
		ELOGM_PRINTLN_MSG("process function handle uninitialized! Please call setProcessFun first to init it!");
		return -1;
	}
	if (setSignal())
	{
		ELOGM_PRINTLN_MSG("set signal error!");
		return -1;
	}
	if (m_maxConnNum > 0)
	{
		if (::sem_init(&m_maxConnNumSem, 0, m_maxConnNum) < 0)
		{
			ELOGM_PRINTLN_ERR("sem_init error", FUtil::getErrCode(), FUtil::getErrStr());
			return -1;
		}
	}
	m_inited = true;
	return 0;
}

int FServer::setSignal()
{
	return 0; // now, I can't solve accept() call restart across signals ...
#ifdef __WIN32__
	if (::signal(SIGINT, FServer::sigHandler) == SIG_ERR)
	{
		return -1;
	}
#else
	struct sigaction new_act;
	new_act.sa_handler = FServer::sigHandler;
	sigemptyset(&new_act.sa_mask);
	new_act.sa_flags = 0;
	if (::sigaction(SIGINT, &new_act, NULL) < 0)
	{
		return -1;
	}
#endif
	return 0;
}

void FServer::sigHandler(int signum)
{
	switch (signum)
	{
		case SIGINT:
			g_server->m_running = false;
			break;
		default:
			break;
	}
}

/*
 * setProcessFun:
 * Set the process function for each connection.
 * The process fun must release Socket* send to it.
 */
void FServer::setProcessFun(const fs_fun_t fun)
{
	this->m_funHandle = fun;
}

fs_fun_t FServer::getProcessFun()
{
	return this->m_funHandle;
}

void FServer::setListenQueueLen(int queue_len)
{
	this->m_listenQueueLen = queue_len;
}

int FServer::getListenQueueLen()
{
	return this->m_listenQueueLen;
}

int FServer::getMaxConnNum()
{
	return this->m_maxConnNum;
}

int FServer::run()
{
	if (!m_inited && init())
	{
		ELOGM_PRINTLN_MSG("server init failed!");
		return -1;
	}
	int ret = m_serverSocket.bind(this->m_addr.empty() ? NULL : this->m_addr.c_str(), (in_port_t) this->m_port);
	if (ret < 0)
	{
		ELOGM_PRINTLN_ERR("server bind error", m_serverSocket.getErrCode(), m_serverSocket.getErrStr());
		return ret;
	}
	ret = m_serverSocket.listen(m_listenQueueLen);
	if (ret < 0)
	{
		ELOGM_PRINTLN_ERR("server listen error", m_serverSocket.getErrCode(), m_serverSocket.getErrStr());
		return ret;
	}

	DLOGM_PRINT("server listening on %s:%d ...\n", this->m_addr.empty() ? "*" : this->m_addr.c_str(), this->m_port);

	ret = this->loop();

	m_serverSocket.close();
	m_running = false;

	return ret;
}

int FServer::loop()
{
	m_running = true;
	while (m_running)
	{
		if (m_maxConnNum > 0 && ::sem_wait(&m_maxConnNumSem) < 0)
		{
			ELOGM_PRINTLN_MSG("sem_wait error!");
			break;
		}
		//DEBUG_PRINTLN_MSG("waitting for connection ...");
		FSocketTcp *socket = m_serverSocket.accept();
		if (socket)
		{
			//DEBUG_MPRINT("accept a connection, sid: %d.\n", socket->getSocketHandle());
			m_threadPool.pushTask(new FServerTask(this, socket));
			//sleep(1000);
		}
		else
		{
			int errcode = m_serverSocket.getErrCode();
			if (errcode == EINTR)
			{
				continue;
			}
			ELOGM_PRINTLN_ERR("server accept error", m_serverSocket.getErrCode(), m_serverSocket.getErrStr());
			return -1;
		}
	}
	return 0;
}

void FServer::taskDone(FSocketTcp *p_socket)
{
	if (::sem_post(&m_maxConnNumSem) < 0)
	{
		ELOGM_PRINTLN_MSG("sem_post error!");
	}
#if _DEBUG_
	m_threadPool.printStatus();
#endif
}

} /* namespace freeyxm */

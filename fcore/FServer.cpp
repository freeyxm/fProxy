/*
 * FServer.cpp
 *
 *  Created on: 2012-10-24
 *      Author: kill
 */

#include "FServer.h"
#include "FUtil.h"
#include "FThread.h"
#include <signal.h>
#include <cstdlib>
#if __linux__
#include <errno.h>
#endif

namespace freeyxm {

static FServer *g_server;

typedef struct {
	sem_t *p_sem;
	ft_funparam_t funparam;
} fs_tp_param_t;

FServer::FServer(const string addr, const int port, const unsigned int max_conn_num) :
		addr(addr), port(port), max_conn_num(max_conn_num) {
	funHandle = NULL;
	stopped = 1;
	listen_queue_len = 10;
	g_server = this;
	p_max_conn_num_sem = NULL;
}

FServer::~FServer() {
	if (p_max_conn_num_sem) {
		::free(p_max_conn_num_sem);
		p_max_conn_num_sem = NULL;
	}
}

int FServer::init() {
	if (!this->funHandle) {
		DEBUG_PRINTLN_MSG("process function handle uninitialized!"
				" Please call setProcessFun first to init it!");
		return -1;
	}
	if (setSignal()) {
		DEBUG_PRINTLN_MSG("set signal error!");
		return -1;
	}
	if (max_conn_num > 0) {
		p_max_conn_num_sem = (sem_t*) malloc(sizeof(sem_t));
		if (!p_max_conn_num_sem) {
			DEBUG_PRINTLN_MSG("malloc <p_max_conn_num_sem> error!");
			return -1;
		}
		if (::sem_init(p_max_conn_num_sem, 0, max_conn_num) < 0) {
			DEBUG_PRINTLN_MSG("sem_init error!");
			return -1;
		}
	}
	return 0;
}

int FServer::setSignal() {
	return 0; // now, I can't solve accept() call restart across signals ...
#ifdef __WIN32__
	if (::signal(SIGINT, FServer::sigHandler) == SIG_ERR ) {
		return -1;
	}
#else
	struct sigaction new_act;
	new_act.sa_handler = FServer::sigHandler;
	sigemptyset(&new_act.sa_mask);
	new_act.sa_flags = 0;
	if (::sigaction(SIGINT, &new_act, NULL) < 0) {
		return -1;
	}
#endif
	return 0;
}

void FServer::sigHandler(int signum) {
	switch (signum) {
	case SIGINT:
		g_server->stopped = 1;
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
void FServer::setProcessFun(const fs_fun_t fun) {
	this->funHandle = fun;
}

void FServer::setListenQueueLen(int queue_len) {
	this->listen_queue_len = queue_len;
}

int FServer::getListenQueueLen() {
	return this->listen_queue_len;
}

int FServer::getMaxConnNum() {
	return this->max_conn_num;
}

int FServer::run() {
	if (init()) {
		DEBUG_PRINTLN_MSG("server init failed!");
		return -1;
	}
	int ret = server_socket.bind(this->addr.empty() ? NULL : this->addr.c_str(), this->port);
	if (ret < 0) {
		DEBUG_PRINTLN_ERR("server bind error", server_socket.getErrCode(), server_socket.getErrStr().c_str());
		return ret;
	}
	ret = server_socket.listen(listen_queue_len);
	if (ret < 0) {
		DEBUG_PRINTLN_ERR("server listen error", server_socket.getErrCode(), server_socket.getErrStr().c_str());
		return ret;
	}

	DEBUG_PRINT_T(1, "server listening on %s:%d ...\n", this->addr.empty() ? "*" : this->addr.c_str(), this->port);

	ret = this->loop();

	server_socket.close();
	stopped = 1;

	return ret;
}

int FServer::loop() {
	stopped = 0;
	while (!stopped) {
		if (p_max_conn_num_sem && ::sem_wait(p_max_conn_num_sem) < 0) {
			DEBUG_PRINTLN_MSG("sem_wait error!");
			break;
		}
		//DEBUG_PRINTLN_MSG("waitting for connection ...");
		FSocketTcp *socket = server_socket.accept();
		if (socket) {
			//DEBUG_PRINT_T(1, "accept a connection, sid: %d.\n", socket->getSocketHandle());
			if (this->process(socket)) {
				return -1;
			}
		} else {
			int errcode = server_socket.getErrCode();
			if (errcode == EINTR) {
				continue;
			}
			DEBUG_PRINTLN_ERR("server accept error", server_socket.getErrCode(), server_socket.getErrStr().c_str());
			return -1;
		}
	}
	return 0;
}

int FServer::process(FSocketTcp *p_socket) {
	// ---
	fs_tp_param_t *tp_param = (fs_tp_param_t*) ::malloc(sizeof(fs_tp_param_t));
	if (!tp_param) {
		DEBUG_PRINTLN_MSG("malloc error!");
		return -1;
	}
	tp_param->p_sem = this->p_max_conn_num_sem;
	tp_param->funparam.handle = (ft_fun_t) this->funHandle;
	tp_param->funparam.param = (void*) p_socket;
	// ---
	ft_funparam_t funparam = { (ft_fun_t) FServer::thread_proxy, (void*) tp_param };
	FThread thread(&funparam);
	int ret = thread.start();
	if (ret) {
		DEBUG_PRINT_T(1, "thread create error, sid: %d, ret: %d.\n", p_socket->getSocketHandle(), ret);
		free(tp_param); // !!!
		return -1;
	} else {
		thread.detach(); // ...
	}
	return 0;
}

void FServer::thread_proxy(void *param) {
	fs_tp_param_t *tp_param = (fs_tp_param_t*) param;

	do {
		/*
		 // ::sem_post call in loop() ...
		 if (tp_param->p_sem) {
		 if (::sem_wait(tp_param->p_sem) < 0) {
		 DEBUG_PRINTLN_MSG("sem_wait error!");
		 }
		 }
		 */
		(*tp_param->funparam.handle)(tp_param->funparam.param);
		if (tp_param->p_sem) {
			if (::sem_post(tp_param->p_sem) < 0) {
				DEBUG_PRINTLN_MSG("sem_post error!");
			}
		}
	} while (0);

	::free(tp_param);
}

} /* namespace freeyxm */

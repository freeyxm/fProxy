/*
 * FProtocol.cpp
 *
 *  Created on: 2012-11-5
 *      Author: AKA
 */

#include "FProtocol.h"
#include "fcore/FUtil.h"
#include "fcore/FString.h"
#include <cstdio>
#include <cstdlib>
#include <tr1/memory>
#ifdef __WIN32__
#include <winsock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

namespace freeyxm {

FProtocol::FProtocol()
{
}

FProtocol::~FProtocol()
{
}

void FProtocol::inet_ntop(int af, const void *cp, string &addr)
{
	char buf[40];
	::inet_ntop(af, cp, buf, sizeof(buf));
	addr = buf;
}

unsigned short FProtocol::ntohs(const unsigned char *p_ns)
{
	return ::ntohs(*((unsigned short*) p_ns));
}

void FProtocol::htons(const unsigned short hs, char *p_ns)
{
	*((unsigned short*) p_ns) = ::htons(hs);
}

void FProtocol::ntons(const unsigned short ns, char *p_ns)
{
	*(unsigned short*) p_ns = ns;
}

/*
 * onceRecvAndSend
 * return: 1, success; 0, recv_socket closed; -1, error.
 */
int FProtocol::onceRecvAndSend(FSocketTcp *recv_socket, FSocketTcp *send_socket, bool send_flag, bool recv_flag)
{
	static char __thread buf[RELAY_BUF_SIZE];
	static int __thread send_num = 0, recv_num = 0;

	int ret = Sock_Done;
	int nsend = 0, nrecv = 0;
	bool send_over = false, recv_over = false;
	do
	{
		if (send_flag && !send_over)
		{
			while (send_num < recv_num)
			{
				nsend = send_socket->send(buf + send_num, recv_num - send_num);
				if (nsend < 0)
				{
					int errCode = send_socket->getErrCode();
					if (errCode == EAGAIN || errCode == EWOULDBLOCK)
					{
						send_over = true;
						break;
					}
					ret = Sock_Error;
					DLOGM_PRINTLN_ERR("send error", send_socket->getErrCode(), send_socket->getErrStr());
					break;
				}
				send_num += nsend;
				//DLOGM_PRINT_T("send %d bytes: %d -> %d\n", nsend, recv_socket->getHandle(), send_socket->getHandle());
			}
			if (ret == Sock_Error)
				break;
			if (send_num >= recv_num)
				send_num = recv_num = 0;
		} // end send

		if (recv_flag && !recv_over)
		{
			while (recv_num < RELAY_BUF_SIZE)
			{
				nrecv = recv_socket->recv(buf + recv_num, RELAY_BUF_SIZE - recv_num);
				if (nrecv < 0)
				{
					int errCode = recv_socket->getErrCode();
					if (errCode == EAGAIN || errCode == EWOULDBLOCK)
					{
						recv_over = true;
						break;
					}
					ret = Sock_Error;
					DLOGM_PRINTLN_ERR("recv error", recv_socket->getErrCode(), recv_socket->getErrStr());
					break;
				}
				else if (nrecv == 0)
				{
					ret = Sock_CLosed;
					break; // remote socket closed.
				}
				recv_num += nrecv;
				//DLOGM_PRINT_T("recv %d bytes: %d -> %d\n", nrecv, recv_socket->getHandle(), send_socket->getHandle());
			}

			if (ret == Sock_Error || ret == Sock_CLosed)
				break;

			if (send_num == 0 && recv_num > 0)
				send_flag = true;
		} // end recv

		if ((send_over || send_num >= recv_num) && (recv_over || recv_num >= RELAY_BUF_SIZE))
			break;
	} while (true);

	if (ret == Sock_Error || ret == Sock_CLosed)
	{
		send_num = recv_num = 0;
	}
	else
	{
		if (send_num < recv_num)
			ret |= Sock_Write;
		if (recv_num < RELAY_BUF_SIZE)
			ret |= Sock_Read;
	}
	return ret;
}

int FProtocol::loopRecvAndSend(FSocketTcp *c_sock, FSocketTcp *s_sock)
{
	if (c_sock->setBlockMode(false) < 0)
	{
		DLOGM_PRINTLN_ERR("setBlockMode error", c_sock->getErrCode(), c_sock->getErrStr());
		return -1;
	}
	if (s_sock->setBlockMode(false) < 0)
	{
		DLOGM_PRINTLN_ERR("setBlockMode error", s_sock->getErrCode(), s_sock->getErrStr());
		return -1;
	}

	int c_sid = c_sock->getHandle();
	int s_sid = s_sock->getHandle();
	int nfds = c_sid > s_sid ? c_sid + 1 : s_sid + 1;
	fd_set r_fds, w_fds;

	FD_ZERO(&w_fds);
	FD_ZERO(&r_fds);
	FD_SET(c_sid, &r_fds);
	FD_SET(s_sid, &r_fds);

	int select_ret;
	while (1)
	{
		select_ret = ::select(nfds, &r_fds, &w_fds, NULL, NULL);
		if (select_ret < 0)
		{
			DLOGM_PRINTLN_ERR("select error", FUtil::getErrCode(), FUtil::getErrStr());
			break;
		}
		else if (select_ret == 0)
		{ // should't happen here!
			break;
		}

		bool client_recv = FD_ISSET(c_sid, &r_fds);
		bool client_send = FD_ISSET(c_sid, &w_fds);
		bool server_recv = FD_ISSET(s_sid, &r_fds);
		bool server_send = FD_ISSET(s_sid, &w_fds);
		FD_ZERO(&r_fds);
		FD_ZERO(&w_fds);

		if (client_recv || server_send)
		{
			int ret = FProtocol::onceRecvAndSend(c_sock, s_sock, server_send, client_recv);
			if (ret <= 0)
				break;
			if (ret & Sock_Read)
				FD_SET(c_sid, &r_fds);
			if (ret & Sock_Write)
				FD_SET(s_sid, &w_fds);
		}
		else
		{
			FD_SET(c_sid, &r_fds);
		}
		if (server_recv || client_send)
		{
			int ret = FProtocol::onceRecvAndSend(s_sock, c_sock, client_send, server_recv);
			if (ret <= 0)
				break;
			if (ret & Sock_Read)
				FD_SET(s_sid, &r_fds);
			if (ret & Sock_Write)
				FD_SET(c_sid, &w_fds);
		}
		else
		{
			FD_SET(s_sid, &r_fds);
		}
	} // end while

	return 0;
}

} /* namespace freeyxm */

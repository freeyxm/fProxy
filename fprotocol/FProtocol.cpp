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
	// TODO Auto-generated constructor stub

}

FProtocol::~FProtocol()
{
	// TODO Auto-generated destructor stub
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
int FProtocol::onceRecvAndSend(FSocketTcp *recv_socket, FSocketTcp *send_socket, char *buf, const int buf_size)
{
	int nrecv = 0, nsend = 0;
	int ret = 1;
	do
	{
		nrecv = recv_socket->recv(buf, buf_size);
		if (nrecv < 0)
		{
			int errCode = recv_socket->getErrCode();
			if (errCode == EAGAIN || errCode == EWOULDBLOCK)
				break;

			DLOGM_PRINTLN_ERR("recv error", recv_socket->getErrCode(), recv_socket->getErrStr());
			ret = -1;
			break;
		}
		else if (nrecv == 0)
		{
			ret = 0;
			break; // remote socket closed.
		}

		nsend = send_socket->send(buf, nrecv);
		if (nsend < 0 || nsend != nrecv)
		{
			DLOGM_PRINTLN_ERR("send error", send_socket->getErrCode(), send_socket->getErrStr());
			ret = -1;
			break;
		}

		if (nrecv < buf_size)
			break;
	} while (true);

	return ret;
}

int FProtocol::loopRecvAndSend(FSocketTcp *socket1, FSocketTcp *socket2)
{
	int c_sid = socket1->getHandle();
	int s_sid = socket2->getHandle();
	fd_set r_fds;
	int nfds = c_sid > s_sid ? c_sid + 1 : s_sid + 1;

	std::tr1::shared_ptr<char> relay_buf(new char[RELAY_BUF_SIZE + 1]);
	if (!relay_buf.get())
	{
		DLOGM_PRINTLN_MSG("new relay_buf failed!");
		return -1;
	}

	if (socket1->setBlockMode(0) < 0)
	{
		DLOGM_PRINTLN_ERR("setBlockMode error", socket1->getErrCode(), socket1->getErrStr());
		return -1;
	}
	if (socket2->setBlockMode(0) < 0)
	{
		DLOGM_PRINTLN_ERR("setBlockMode error", socket2->getErrCode(), socket2->getErrStr());
		return -1;
	}

	int select_ret;
	while (1)
	{
		FD_ZERO(&r_fds);
		FD_SET(c_sid, &r_fds);
		FD_SET(s_sid, &r_fds);
		select_ret = ::select(nfds, &r_fds, NULL, NULL, NULL);
		if (select_ret < 0)
		{
			DLOGM_PRINTLN_ERR("select error", FUtil::getErrCode(), FUtil::getErrStr());
			break;
		}
		else if (select_ret == 0)
		{ // should't happen here!
			break;
		}

		if (select_ret && FD_ISSET(c_sid, &r_fds))
		{
			if (FProtocol::onceRecvAndSend(socket1, socket2, relay_buf.get(), RELAY_BUF_SIZE) <= 0)
				break;
			--select_ret;
		}
		if (select_ret && FD_ISSET(s_sid, &r_fds))
		{
			if (FProtocol::onceRecvAndSend(socket2, socket1, relay_buf.get(), RELAY_BUF_SIZE) <= 0)
				break;
			--select_ret;
		}
	} // end while

	return 0;
}

} /* namespace freeyxm */

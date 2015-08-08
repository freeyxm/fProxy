/*
 * FSocketTcp.cpp
 *
 *  Created on: 2012-11-10
 *      Author: AKA
 */

#include "FSocketTcp.h"
#include <fcore/FLogger.h>
#include <fcore/FUtil.h>

namespace freeyxm {

FSocketTcp::FSocketTcp(int domain) :
		FSocket(domain, SOCK_STREAM)
{
	// TODO Auto-generated constructor stub
}

FSocketTcp::~FSocketTcp()
{
	// TODO Auto-generated destructor stub
}

int FSocketTcp::listen(const int n)
{
	return ::listen(this->m_sockfd, n);
}

FSocketTcp* FSocketTcp::accept()
{
	sockaddr_storage addr;
#ifdef __WIN32__
	int length = sizeof(addr);
#else
	socklen_t length = sizeof(addr);
#endif
	int ret = ::accept(this->m_sockfd, (struct sockaddr*) &addr, &length);
	if (ret < 0)
	{
		DEBUG_PRINTLN_ERR("accept error", FUtil::getErrCode(), FUtil::getErrStr());
		return NULL;
	}

	FSocketTcp* socket = new FSocketTcp(addr.ss_family);
	if (socket)
	{
		socket->m_sockfd = ret;
		socket->setRemoteAddress((struct sockaddr*) &addr);
	}
	return socket;
}

int FSocketTcp::recv(char *buf, const size_t size)
{
	return ::recv(this->m_sockfd, buf, size, 0);
}

int FSocketTcp::send(const char *buf, const size_t size)
{
	return ::send(this->m_sockfd, buf, size, 0);
}

int FSocketTcp::sendAll(const char *buf, const size_t size, size_t *pSend)
{
	int nsend = 0;
	size_t ntotal = 0;

	while (ntotal < size)
	{
		nsend = ::send(this->m_sockfd, buf + ntotal, size - ntotal, 0);
		if (nsend < 0)
		{
			break;
		}
		ntotal += nsend;
	}

	if (pSend != NULL)
	{
		*pSend = ntotal;
	}

	return nsend <= 0 ? nsend : ntotal;
}

int FSocketTcp::recvAll(char *buf, const size_t size, size_t *pRecv)
{
	int nrecv = 0;
	size_t ntotal = 0;

	while (ntotal < size)
	{
		nrecv = ::recv(this->m_sockfd, buf + ntotal, size - ntotal, 0);
		if (nrecv < 0)
		{
			break;
		}
		else if (nrecv == 0) // socket closed.
		{
			break;
		}
		ntotal += nrecv;
	}

	if (pRecv != NULL)
	{
		*pRecv = ntotal;
	}

	return nrecv <= 0 ? nrecv : ntotal;
}

int FSocketTcp::readLine(string &line, const string line_delimiter)
{
	line.clear();
	char ch;
	int ret;
	int ld_index = 0;
	while (true)
	{
		ret = ::recv(this->m_sockfd, &ch, 1, 0);
		if (ret < 0)
		{
			return -1;
		}
		else if (ret == 0)
		{
			return 0;
		}
		if (ch == line_delimiter[ld_index])
		{
			++ld_index;
			if (!line_delimiter[ld_index])
			{
				break;
			}
			else
			{
				continue;
			}
		}
		else
		{
			if (ld_index)
			{
				line.append(line_delimiter.c_str(), ld_index);
				ld_index = 0;
			}
			line += ch;
		}
	}
	return 1;
}

} /* namespace freeyxm */

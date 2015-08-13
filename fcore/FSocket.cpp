/*
 * FSocket.cpp
 *
 *  Created on: 2012-10-25
 *      Author: kill
 */

#include "FSocket.h"
#include <fcore/FThread.h>
#include <fcore/FSocketDomain4.h>
#include <fcore/FSocketDomain6.h>
#include <fcore/FLogger.h>
#include <fcore/FUtil.h>
#include <cstring>
#include <ctime>
#include <assert.h>
#ifdef __linux__
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif

namespace freeyxm {

FSocket::FSocket(int domain, int type) :
		m_sockfd(-1), m_domain(domain), m_socktype(type), m_pSocketDomain(NULL)
{
	initSocketDomain(domain, type);

	// Notice: winsock, before call socket(...), must call WSAStartup(...) first!
}

FSocket::~FSocket()
{
	this->close();

	if (m_pSocketDomain != NULL)
	{
		delete m_pSocketDomain;
		m_pSocketDomain = NULL;
	}
}

bool FSocket::initSocketDomain(int domain, int type)
{
	if (m_pSocketDomain == NULL)
	{
		if (domain == AF_INET6)
			m_pSocketDomain = new FSocketDomain6(type);
		else if (domain == AF_INET)
			m_pSocketDomain = new FSocketDomain4(type);
		else
			assert(domain == AF_UNSPEC);
	}
	return m_pSocketDomain != NULL;
}

int FSocket::createSocket()
{
	if (m_pSocketDomain == NULL)
	{
		ELOGM_PRINTLN_FL("m_pSocketDomain not init");
		return -1;
	}
	int ret = m_pSocketDomain->createSocket();
	if (ret == 0)
	{
		m_sockfd = m_pSocketDomain->getSocketFd();
	}
	return ret;
}

int FSocket::bind(const char *addr, const in_port_t port)
{
	int ret = -1;
	if (m_pSocketDomain != NULL)
	{
		ret = m_pSocketDomain->bind(addr, port);
	}
	else if (m_domain == AF_UNSPEC)
	{
		struct addrinfo *res;
		if (FSocketDomain::getAddrInfo(m_domain, m_socktype, addr, port, &res) == 0)
		{
			if (initSocketDomain(res->ai_family, res->ai_socktype))
			{
				ret = m_pSocketDomain->bind(res);
			}
			::freeaddrinfo(res);
		}
	}
	if (ret == 0)
	{
		m_sockfd = m_pSocketDomain->getSocketFd();
	}
	return ret;
}

int FSocket::connect(const char *addr, const in_port_t port)
{
	int ret = -1;
	if (m_pSocketDomain != NULL)
	{
		ret = m_pSocketDomain->connect(addr, port);
	}
	else if (m_domain == AF_UNSPEC)
	{
		struct addrinfo *res;
		if (FSocketDomain::getAddrInfo(m_domain, m_socktype, addr, port, &res) == 0)
		{
			if (initSocketDomain(res->ai_family, res->ai_socktype))
			{
				ret = m_pSocketDomain->connect(res);
			}
			::freeaddrinfo(res);
		}
	}
	if (ret == 0)
	{
		m_sockfd = m_pSocketDomain->getSocketFd();
	}
	return ret;
}

void FSocket::close()
{
	if (m_pSocketDomain != NULL)
	{
		m_pSocketDomain->close();
	}
}

int FSocket::recv(char *buf, const size_t size, int flags)
{
	return ::recv(this->m_sockfd, buf, size, flags);
}

int FSocket::send(const char *buf, const size_t size, int flags)
{
	return ::send(this->m_sockfd, buf, size, flags);
}

int FSocket::recvAll(char *buf, const size_t size, int flags, size_t *pRecv)
{
	int nrecv = 0;
	size_t ntotal = 0;

	while (ntotal < size)
	{
		nrecv = ::recv(this->m_sockfd, buf + ntotal, size - ntotal, flags);
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

int FSocket::sendAll(const char *buf, const size_t size, int flags, size_t *pSend)
{
	int nsend = 0;
	size_t ntotal = 0;

	while (ntotal < size)
	{
		nsend = ::send(this->m_sockfd, buf + ntotal, size - ntotal, flags);
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

int FSocket::getHandle()
{
	return this->m_sockfd;
}

int FSocket::setBlockMode(bool block)
{
#ifdef __WIN32__
	u_long mode = block ? 0 : 1;
	if (::ioctlsocket(this->m_sockfd, FIONBIO, &mode))
	{
		ELOGM_PRINTLN_ERR("ioctl error", FUtil::getErrCode(), FUtil::getErrStr());
		return -1;
	}
#else
	int mode = block ? 0 : 1;
	if (::ioctl(this->m_sockfd, FIONBIO, &mode))
	{
		ELOGM_PRINTLN_ERR("ioctl error", FUtil::getErrCode(), FUtil::getErrStr());
		return -1;
	}
#endif
	return 0;
}

int FSocket::setTimeout(bool send_flag, int sec, long usec)
{
	int optname = send_flag ? SO_SNDTIMEO : SO_RCVTIMEO;
	struct timeval time;
	time.tv_sec = sec;
	time.tv_usec = usec;

	int ret = ::setsockopt(this->m_sockfd, SOL_SOCKET, optname, (char*) &time, sizeof(time));
	if (ret != 0)
	{
		ELOGM_PRINTLN_ERR("setsockopt error", FUtil::getErrCode(), FUtil::getErrStr());
	}
	return ret;
}

sockaddr* FSocket::getLocalAddress()
{
	return m_pSocketDomain->getLocalAddress();
}

sockaddr* FSocket::getRemoteAddress()
{
	return m_pSocketDomain->getRemoteAddress();
}

void FSocket::setLocalAddress(const sockaddr* addr)
{
	m_pSocketDomain->setLocalAddress(addr);
}

void FSocket::setRemoteAddress(const sockaddr* addr)
{
	m_pSocketDomain->setRemoteAddress(addr);
}

socklen_t FSocket::getAddrLen()
{
	return m_pSocketDomain->getAddrLen();
}

int FSocket::getErrCode()
{
	return FUtil::getErrCode(); // need to repair!!!
}

const char* FSocket::getErrStr()
{
	return FUtil::getErrStr(); // need to repair!!!
}

int FSocket::setSockaddr(struct sockaddr *pSockAddr, const char *addr, const in_port_t port)
{
	struct addrinfo *res;
	int domain = m_pSocketDomain->getSocketDomain();
	int type = m_pSocketDomain->getSocketType();

	if (FSocketDomain::getAddrInfo(domain, type, addr, port, &res) != 0)
	{
		return -1;
	}

	socklen_t arrdLen = res->ai_addrlen;
	::memcpy(pSockAddr, res->ai_addr, res->ai_addrlen);
	::freeaddrinfo(res);

	return arrdLen;
}

} /* namespace freeyxm */

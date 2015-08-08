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
#if __linux__
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif

namespace freeyxm {

FSocket::FSocket(int domain, int type) :
		m_sockfd(-1)
{
	if (domain == AF_INET6)
		m_pSocketDomain = new FSocketDomain6(type);
	else
		m_pSocketDomain = new FSocketDomain4(type);

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

int FSocket::createSocket()
{
	return m_pSocketDomain->createSocket();
}

int FSocket::bind(const char *addr, const in_port_t port)
{
	int ret = m_pSocketDomain->bind(addr, port);
	if (ret == 0)
	{
		m_sockfd = m_pSocketDomain->getSocketFd();
	}
	return ret;
}

int FSocket::connect(const char *addr, const in_port_t port)
{
	int ret = m_pSocketDomain->connect(addr, port);
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

int FSocket::getHandle()
{
	return this->m_sockfd;
}

int FSocket::setBlockMode(bool block)
{
#ifdef __WIN32__
	u_long mode = block ? 1 : 0;
	if (::ioctlsocket(this->m_sockfd, FIONBIO, &mode))
	{
		return -1;
	}
#else
	int mode = block ? 1 : 0;
	if (::ioctl(this->m_sockfd, FIONBIO, &mode))
	{
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
	return ::setsockopt(this->m_sockfd, SOL_SOCKET, optname, (char*) &time, sizeof(time));
}

sockaddr* FSocket::getRemoteAddress()
{
	return m_pSocketDomain->getRemoteAddress();
}

sockaddr* FSocket::getLocalAddress()
{
	return m_pSocketDomain->getLocalAddress();
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

	::memcpy(pSockAddr, res->ai_addr, res->ai_addrlen);
	::freeaddrinfo(res);

	return 0;
}

} /* namespace freeyxm */

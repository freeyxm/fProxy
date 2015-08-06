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
	return -1;
}

int FSocket::setSockaddr(sockaddr_in &addr, const int sin_family, const char *host, const unsigned int port)
{
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	if (host)
	{
		addr.sin_addr.s_addr = ::inet_addr(host);
	}
	else
	{
		addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	}
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		/* domain name maybe */
		struct hostent *host_info = ::gethostbyname(host);
		if (!host_info)
		{
			return -1;
		}
		addr.sin_addr.s_addr = *(size_t*) host_info->h_addr_list[0];
	}
	return 0;
}

int FSocket::bind(const char *addr, const uint16_t port)
{
	int ret = m_pSocketDomain->bind(addr, port);
	if (ret == 0)
	{
		m_sockfd = m_pSocketDomain->getSocketFd();
	}
	return ret;
}

int FSocket::connect(const char *addr, const uint16_t port)
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

int FSocket::setBlockMode(int flag)
{
	if (flag && flag != 1)
	{
		flag = 1;
	}
#ifdef __WIN32__
	u_long mode = flag;
	if (::ioctlsocket(this->m_sockfd, FIONBIO, &mode))
	{
		return -1;
	}
#else
	int mode = flag;
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
#ifdef __WIN32__
	return ::WSAGetLastError();
#else
	return errno;
#endif
}

string FSocket::getErrStr()
{
#ifdef __WIN32__
	const int BUF_SIZE = 1024;
	char buf[BUF_SIZE + 1];
	buf[BUF_SIZE] = '\0';
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, this->getErrCode(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, BUF_SIZE, NULL);
	return buf;
#else
	return ::strerror(errno);
#endif
}

} /* namespace freeyxm */

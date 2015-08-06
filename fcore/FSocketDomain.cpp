/*
 * FSocketDomain.cpp
 *
 *  Created on: 2015年8月6日
 *      Author: aka
 */

#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE
#define _POSIX_SOURCE

#include <fcore/FSocketDomain.h>
#include <fcore/FUtil.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

namespace freeyxm {

FSocketDomain::FSocketDomain(int socket_domain, int socket_type) :
		m_socketfd(-1), m_socketDomain(socket_domain), m_socketType(socket_type), m_socketProtocol(0)
{
	// TODO Auto-generated constructor stub
}

FSocketDomain::~FSocketDomain()
{
	close();
}

void FSocketDomain::close()
{
	if (this->m_socketfd != -1)
	{
#ifdef __WIN32__
		::closesocket(this->m_socketfd);
#else
		::close(this->m_socketfd);
#endif
		this->m_socketfd = -1;
	}
}

int FSocketDomain::createSocket(int protocol)
{
	if (m_socketfd != -1)
	{
		close();
	}

	m_socketfd = ::socket(this->m_socketDomain, this->m_socketType, protocol);
	if (m_socketfd == -1)
	{
		DEBUG_PRINTLN_ERR("create socket error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
		return -1;
	}

	return 0;
}

int FSocketDomain::getAddrInfo(const char *addr, const uint16_t port, struct addrinfo **res)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = this->m_socketDomain;
	hints.ai_socktype = this->m_socketType;
	if (addr == NULL)
	{
		hints.ai_flags = AI_PASSIVE;
	}

	char port_str[6];
	sprintf(port_str, "%d", port);

	int status = getaddrinfo(addr, port_str, &hints, res);
	if (status != 0)
	{
		DEBUG_PRINTLN_ERR("getaddrinfo error", status, gai_strerror(status));
		return -1;
	}

	return 0;
}

int FSocketDomain::bind(const char *addr, const uint16_t port)
{
	struct addrinfo *res;
	if (getAddrInfo(addr, port, &res) != 0)
	{
		return -1;
	}

	int ret = 0;
	do
	{
		ret = createSocket(res->ai_protocol);
		if (ret != 0)
			break;

		int optval = 1;
		ret = ::setsockopt(this->m_socketfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		if (ret != 0)
		{
			DEBUG_PRINTLN_ERR("setsockopt error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
			break;
		}

		ret = ::bind(this->m_socketfd, res->ai_addr, res->ai_addrlen);
		if (ret != 0)
		{
			DEBUG_PRINTLN_ERR("bind error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
			break;
		}

		setLocalAddress(res->ai_addr);

	} while (false);

	::freeaddrinfo(res);

	return ret == 0 ? 0 : -1;
}

int FSocketDomain::connect(const char *addr, const uint16_t port)
{
	struct addrinfo *res;
	if (getAddrInfo(addr, port, &res) != 0)
	{
		return -1;
	}

	int ret = 0;
	do
	{
		ret = createSocket(res->ai_protocol);
		if (ret != 0)
			break;

		ret = ::connect(this->m_socketfd, res->ai_addr, res->ai_addrlen);
		if (ret != 0)
		{
			DEBUG_PRINTLN_ERR("connect error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
			break;
		}

		setRemoteAddress(res->ai_addr);

	} while (false);

	::freeaddrinfo(res);

	return ret == 0 ? 0 : -1;
}

struct sockaddr* FSocketDomain::getLocalAddress()
{
	return (struct sockaddr*) (&this->m_localAddress);
}

struct sockaddr* FSocketDomain::getRemoteAddress()
{
	return (struct sockaddr*) (&this->m_remoteAddress);
}

} /* namespace freeyxm */

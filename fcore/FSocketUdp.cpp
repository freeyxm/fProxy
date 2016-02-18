/*
 * FSocketUdp.cpp
 *
 *  Created on: 2012-11-10
 *      Author: AKA
 */

#include "FSocketUdp.h"
#include <assert.h>

namespace freeyxm {

FSocketUdp::FSocketUdp(int domain) :
		FSocket(domain, SOCK_DGRAM)
{
	m_remoteAddrLen = this->getAddrLen();
}

FSocketUdp::~FSocketUdp()
{
}

int FSocketUdp::createSocket()
{
	return FSocket::createSocket();
}

int FSocketUdp::recvFrom(char *buf, const size_t size, struct sockaddr *addr, socklen_t *addrLen)
{
	return ::recvfrom(this->m_sockfd, buf, size, 0, addr, addrLen);
}

int FSocketUdp::sendTo(const char *buf, const size_t size, const struct sockaddr *addr, const socklen_t addrLen)
{
	return ::sendto(this->m_sockfd, buf, size, 0, addr, addrLen);
}

int FSocketUdp::recvFrom(char *buf, const size_t size)
{
	return this->recvFrom(buf, size, this->getRemoteAddress(), &m_remoteAddrLen);
}

int FSocketUdp::sendTo(const char *buf, const size_t size)
{
	return this->sendTo(buf, size, this->getRemoteAddress(), m_remoteAddrLen);
}

int FSocketUdp::sendTo(const char *buf, const size_t size, const char *host, const in_port_t port)
{
	int addrLen = this->setSockAddr(this->getRemoteAddress(), host, port);
	if (addrLen < 0)
	{
		return -1;
	}
	m_remoteAddrLen = addrLen;
	return this->sendTo(buf, size, this->getRemoteAddress(), m_remoteAddrLen);
}

} /* namespace freeyxm */

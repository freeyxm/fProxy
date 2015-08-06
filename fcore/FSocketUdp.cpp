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
	// TODO Auto-generated constructor stub
}

FSocketUdp::~FSocketUdp()
{
	// TODO Auto-generated destructor stub
}

int FSocketUdp::createSocket()
{
	return FSocket::createSocket();
}

int FSocketUdp::recvFrom(char *buf, const size_t size, struct sockaddr *addr)
{
	unsigned int addrLen = this->getAddrLen();
	return ::recvfrom(this->m_sockfd, buf, size, 0, addr, &addrLen);
}

int FSocketUdp::sendTo(const char *buf, const size_t size, const struct sockaddr *addr)
{
	return ::sendto(this->m_sockfd, buf, size, 0, addr, this->getAddrLen());
}

int FSocketUdp::recvFrom(char *buf, const size_t size)
{
	return this->recvFrom(buf, size, this->getRemoteAddress());
}

int FSocketUdp::sendTo(const char *buf, const size_t size)
{
	return this->sendTo(buf, size, this->getRemoteAddress());
}

int FSocketUdp::sendTo(const char *buf, const size_t size, const char *host, const unsigned int port)
{
//	if (this->setSockaddr(this->remoteAddress, this->sin_family, host, port)) {
//		return -1;
//	}
//	return this->sendTo(buf, size, &this->remoteAddress);
	assert(false);
	return -1;
}

} /* namespace freeyxm */

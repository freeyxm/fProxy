/*
 * FSocketDomain6.cpp
 *
 *  Created on: 2015年8月6日
 *      Author: aka
 */

#include <fcore/FSocketDomain6.h>
#include <cstring>

namespace freeyxm {

FSocketDomain6::FSocketDomain6(int socket_type) :
		FSocketDomain(AF_INET6, socket_type)
{
	// TODO Auto-generated constructor stub
}

FSocketDomain6::~FSocketDomain6()
{
	// TODO Auto-generated destructor stub
}

void FSocketDomain6::setLocalAddress(const struct sockaddr* addr)
{
	memcpy(&this->m_localAddress, addr, sizeof(struct sockaddr_in6));
}

void FSocketDomain6::setRemoteAddress(const struct sockaddr* addr)
{
	memcpy(&this->m_remoteAddress, addr, sizeof(struct sockaddr_in6));
}

socklen_t FSocketDomain6::getAddrLen()
{
	return sizeof(struct sockaddr_in6);
}

} /* namespace freeyxm */

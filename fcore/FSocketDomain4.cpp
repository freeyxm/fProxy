/*
 * FSocketDomain4.cpp
 *
 *  Created on: 2015年8月6日
 *      Author: aka
 */

#include <fcore/FSocketDomain4.h>
#include <cstring>

namespace freeyxm {

FSocketDomain4::FSocketDomain4(int socket_type) :
		FSocketDomain(AF_INET, socket_type)
{
}

FSocketDomain4::~FSocketDomain4()
{
}

void FSocketDomain4::setLocalAddress(const struct sockaddr* addr)
{
	memcpy(&this->m_localAddress, addr, sizeof(struct sockaddr_in));
}

void FSocketDomain4::setRemoteAddress(const struct sockaddr* addr)
{
	memcpy(&this->m_remoteAddress, addr, sizeof(struct sockaddr_in));
}

socklen_t FSocketDomain4::getAddrLen()
{
	return sizeof(struct sockaddr_in);
}

} /* namespace freeyxm */

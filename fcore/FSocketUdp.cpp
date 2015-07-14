/*
 * FSocketUdp.cpp
 *
 *  Created on: 2012-11-10
 *      Author: AKA
 */

#include "FSocketUdp.h"

namespace freeyxm {

FSocketUdp::FSocketUdp(int sin_family) :
		FSocket(sin_family) {
	socket_domain = sin_family;
	socket_type = SOCK_DGRAM;
	socket_protocol = IPPROTO_UDP;
}

FSocketUdp::~FSocketUdp() {
	// TODO Auto-generated destructor stub
}

int FSocketUdp::createSocketUdp() {
	return FSocket::createSocket();
}

int FSocketUdp::recvFrom(char *buf, const size_t size, struct sockaddr_in *addr) {
	unsigned int addrLen = sizeof(sockaddr_in);
	return ::recvfrom(this->sock_fd, buf, size, 0, (struct sockaddr*) addr, &addrLen);
}

int FSocketUdp::sendTo(const char *buf, const size_t size, const struct sockaddr_in *addr) {
	return ::sendto(this->sock_fd, buf, size, 0, (struct sockaddr*) addr, sizeof(sockaddr_in));
}

int FSocketUdp::recvFrom(char *buf, const size_t size) {
	return this->recvFrom(buf, size, &this->remoteAddress);
}

int FSocketUdp::sendTo(const char *buf, const size_t size) {
	return this->sendTo(buf, size, &this->remoteAddress);
}

int FSocketUdp::sendTo(const char *buf, const size_t size, const char *host, const unsigned int port) {
	if (this->setSockaddr(this->remoteAddress, this->sin_family, host, port)) {
		return -1;
	}
	return this->sendTo(buf, size, &this->remoteAddress);
}

} /* namespace freeyxm */

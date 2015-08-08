/*
 * FSocketUdp.h
 *
 *  Created on: 2012-11-10
 *      Author: AKA
 */

#ifndef FSOCKETUDP_H_
#define FSOCKETUDP_H_

#include "FSocket.h"

namespace freeyxm {

class FSocketUdp: public freeyxm::FSocket {
public:
	FSocketUdp(int domain = AF_INET);
	virtual ~FSocketUdp();

	int createSocket();

	int recvFrom(char *buf, const size_t size, struct sockaddr *addr, socklen_t *addrLen);
	int sendTo(const char *buf, const size_t size, const struct sockaddr *addr, const socklen_t addrLen);

	int recvFrom(char *buf, const size_t size);
	int sendTo(const char *buf, const size_t size);

	int sendTo(const char *buf, const size_t size, const char *host, const in_port_t port);

protected:
	socklen_t m_remoteAddrLen;
};

} /* namespace freeyxm */
#endif /* FSOCKETUDP_H_ */

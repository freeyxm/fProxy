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
	FSocketUdp(int sin_family = AF_INET);
	virtual ~FSocketUdp();

	int createSocketUdp();

	int recvFrom(char *buf, const size_t size, struct sockaddr_in *addr);
	int sendTo(const char *buf, const size_t size,
			const struct sockaddr_in *addr);

	int recvFrom(char *buf, const size_t size);
	int sendTo(const char *buf, const size_t size);

	int sendTo(const char *buf, const size_t size, const char *host,
			const unsigned int port);
};

} /* namespace freeyxm */
#endif /* FSOCKETUDP_H_ */

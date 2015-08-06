/*
 * FSocketDomain.h
 *
 *  Created on: 2015年8月6日
 *      Author: aka
 */

#ifndef FCORE_FSOCKETDOMAIN_H_
#define FCORE_FSOCKETDOMAIN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>

namespace freeyxm {

class FSocketDomain {
public:
	FSocketDomain(int socket_domain, int socket_type);
	virtual ~FSocketDomain();

	virtual int bind(const char *addr, const uint16_t port);
	virtual int connect(const char *addr, const uint16_t port);
	virtual void close();

	struct sockaddr* getLocalAddress();
	struct sockaddr* getRemoteAddress();
	virtual void setLocalAddress(const struct sockaddr* addr) = 0;
	virtual void setRemoteAddress(const struct sockaddr* addr) = 0;

	virtual socklen_t getAddrLen() = 0;

	inline int getSocketFd()
	{
		return m_socketfd;
	}

protected:
	int createSocket(int protocol);
	int getAddrInfo(const char *addr, const uint16_t port, struct addrinfo **res);

protected:
	int m_socketfd;
	int m_socketDomain;
	int m_socketType;
	int m_socketProtocol;
	struct sockaddr_storage m_localAddress;
	struct sockaddr_storage m_remoteAddress;
};

} /* namespace freeyxm */

#endif /* FCORE_FSOCKETDOMAIN_H_ */

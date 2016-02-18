/*
 * FSocket.h
 *
 *  Created on: 2012-10-25
 *      Author: freeyxm
 */

#ifndef FSOCKET_H_
#define FSOCKET_H_

#include <fcore/FSocketDomain.h>
#ifdef __WIN32__
#include <winsock.h>
#include <wininet.h>
#elif defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <string>

namespace freeyxm {

#ifdef __WIN32__
#define EADDRINUSE WSAEADDRINUSE
#endif

using std::string;

class FSocket {
public:
	FSocket(int domain, int type);
	virtual ~FSocket() = 0;

	virtual int bind(const char *addr, const in_port_t port);
	virtual int connect(const char *addr, const in_port_t port);
	virtual void close();

	virtual int recv(char *buf, const size_t size, int flags = 0);
	virtual int send(const char *buf, const size_t size, int flags = 0);

	virtual int recvAll(char *buf, const size_t size, int flags = 0, size_t *pRecv = NULL);
	virtual int sendAll(const char *buf, const size_t size, int flags = 0, size_t *pSend = NULL);

	int getHandle();

	int setBlockMode(bool block);
	int setTimeout(bool send_flag, int sec, long usec = 0);

	sockaddr* getLocalAddress();
	sockaddr* getRemoteAddress();
	void setLocalAddress(const sockaddr* addr);
	void setRemoteAddress(const sockaddr* addr);

	socklen_t getAddrLen();

	int getErrCode();
	const char* getErrStr();

protected:
	bool initSocketDomain(int domain, int type);
	int createSocket();
	int setSockAddr(struct sockaddr *pSockAddr, const char *addr, const in_port_t port);

protected:
	int m_sockfd;
	int m_domain;
	int m_socktype;
	FSocketDomain *m_pSocketDomain;
};

} /* namespace freeyxm */
#endif /* FSOCKET_H_ */

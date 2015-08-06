/*
 * FSocket.h
 *
 *  Created on: 2012-10-25
 *      Author: freeyxm
 */

#ifndef FSOCKET_H_
#define FSOCKET_H_

#ifdef __WIN32__
#include <winsock.h>
#include <wininet.h>
#elif __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <string>
#include <fcore/FSocketDomain.h>

namespace freeyxm {

#ifdef __WIN32__
#define EADDRINUSE WSAEADDRINUSE
#endif

using std::string;

class FSocket {
public:
	FSocket(int domain, int type);
	virtual ~FSocket() = 0;

	virtual int bind(const char *addr, const uint16_t port);
	virtual int connect(const char *addr, const uint16_t port);
	virtual void close();

	int getHandle();

	int setBlockMode(int flag);

	int setTimeout(bool send_flag, int sec, long usec = 0);

	sockaddr* getLocalAddress();
	sockaddr* getRemoteAddress();
	void setLocalAddress(const sockaddr* addr);
	void setRemoteAddress(const sockaddr* addr);

	socklen_t getAddrLen();

	int getErrCode();
	string getErrStr();

protected:
	int createSocket();
	int setSockaddr(sockaddr_in &addr, const int sin_family, const char *host, const unsigned int port);

protected:
	int m_sockfd;
	FSocketDomain *m_pSocketDomain;
};

} /* namespace freeyxm */
#endif /* FSOCKET_H_ */

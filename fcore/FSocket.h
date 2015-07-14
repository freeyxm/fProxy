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

namespace freeyxm {

#ifdef __WIN32__
#define EADDRINUSE WSAEADDRINUSE
#endif

using std::string;

class FSocket {
public:
	FSocket(int sin_family = AF_INET);
	virtual ~FSocket();

	virtual int bind(const char *addr, const unsigned int port);
	virtual int connect(const char *host, const unsigned int port);
	virtual void close();

	int getSocketHandle();
	int getErrCode();
	string getErrStr();

	int setBlockMode(int flag);

	sockaddr_in getRemoteAddress();
	sockaddr_in getLocalAddress();
	void setRemoteAddress(const sockaddr_in &addr);
	void setLocalAddress(const sockaddr_in &addr);

	static string inet_ntoa(struct in_addr addr);

	int getSinFamily();
	void setSinFamily(int sin_family);

	int setSocketTimeout(int send_flag, int time);

protected:
	int createSocket();
	int setSockaddr(sockaddr_in &addr, const int sin_family, const char *host,
			const unsigned int port);

protected:
	int sock_fd;
	sockaddr_in remoteAddress;
	sockaddr_in localAddress;
	int sin_family;
	int socket_domain;
	int socket_type;
	int socket_protocol;
};

} /* namespace freeyxm */
#endif /* FSOCKET_H_ */

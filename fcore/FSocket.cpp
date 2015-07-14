/*
 * FSocket.cpp
 *
 *  Created on: 2012-10-25
 *      Author: kill
 */

#include "FSocket.h"
#include "FThread.h"
#include <cstring>
#include <ctime>
#if __linux__
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif

namespace freeyxm {

static pthread_mutex_t system_call_mutex = PTHREAD_MUTEX_INITIALIZER;

FSocket::FSocket(int sin_family) :
		sin_family(sin_family) {
	sock_fd = -1;
	socket_domain = 0;
	socket_type = 0;
	socket_protocol = 0;
#ifdef __WIN32__
	// winsock: before call socket(...), must call WSAStartup(...) first!
	// this call by user may be better ...
	/*
	 WSADATA wsa_data;
	 if (WSAStartup((WORD) (1 << 8 | 1), &wsa_data) != 0) {
	 printf("WSAStartup error!\n");
	 }
	 */
#endif
}

FSocket::~FSocket() {
	this->close();
}

int FSocket::createSocket() {
	if (this->sock_fd == -1) {
		this->sock_fd = ::socket(this->socket_domain, this->socket_type, this->socket_protocol);
		if (this->sock_fd == -1) {
			return -1;
		}
	}
	return 0;
}

int FSocket::setSockaddr(sockaddr_in &addr, const int sin_family, const char *host, const unsigned int port) {
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = this->sin_family;
	addr.sin_port = ::htons(port);
	if (host) {
		addr.sin_addr.s_addr = ::inet_addr(host);
	} else {
		addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	}
	if (this->remoteAddress.sin_addr.s_addr == INADDR_NONE) {
		/* domain name maybe */
		struct hostent *host_info = ::gethostbyname(host);
		if (!host_info) {
			return -1;
		}
		this->remoteAddress.sin_addr.s_addr = *(size_t*) host_info->h_addr_list[0];
	}
	return 0;
}

int FSocket::bind(const char *addr, const unsigned int port) {
	if (this->sock_fd == -1) {
		if (this->createSocket() < 0) {
			return -1;
		}
		int opt = 1;
		::setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt));
	}
	if (this->setSockaddr(this->localAddress, this->sin_family, addr, port)) {
		return -1;
	}
	return ::bind(this->sock_fd, (struct sockaddr*) &this->localAddress, sizeof this->localAddress);
}

int FSocket::connect(const char *host, const unsigned int port) {
	if (this->sock_fd == -1) {
		if (this->createSocket() < 0) {
			return -1;
		}
	}
	if (this->setSockaddr(this->remoteAddress, this->sin_family, host, port)) {
		return -1;
	}
	return ::connect(this->sock_fd, (struct sockaddr*) &this->remoteAddress, sizeof this->remoteAddress);
}

void FSocket::close() {
	if (this->sock_fd != -1) {
#ifdef __WIN32__
		::closesocket(this->sock_fd);
#else
		::close(this->sock_fd);
#endif
		this->sock_fd = -1;
	}
}

int FSocket::getSocketHandle() {
	return this->sock_fd;
}

int FSocket::getErrCode() {
#ifdef __WIN32__
	return ::WSAGetLastError();
#else
	return errno;
#endif
}

string FSocket::getErrStr() {
#ifdef __WIN32__
	const int BUF_SIZE = 1024;
	char buf[BUF_SIZE + 1];
	buf[BUF_SIZE] = '\0';
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, this->getErrCode(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, BUF_SIZE, NULL);
	return buf;
#else
	return ::strerror(errno);
#endif
}

int FSocket::setBlockMode(int flag) {
	if (flag) {
		flag = 1;
	}
#ifdef __WIN32__
	u_long mode = flag;
	if (::ioctlsocket(this->sock_fd, FIONBIO, &mode)) {
		return -1;
	}
#else
	int mode = flag;
	if (::ioctl(this->sock_fd, FIONBIO, &mode)) {
		return -1;
	}
#endif
	return 0;
}

sockaddr_in FSocket::getRemoteAddress() {
	return this->remoteAddress;
}

sockaddr_in FSocket::getLocalAddress() {
	return this->localAddress;
}

void FSocket::setRemoteAddress(const sockaddr_in &addr) {
	this->remoteAddress = addr;
}

void FSocket::setLocalAddress(const sockaddr_in &addr) {
	this->localAddress = addr;
}

string FSocket::inet_ntoa(struct in_addr addr) {
	string addr_str;
	::pthread_mutex_lock(&system_call_mutex);
	addr_str = ::inet_ntoa(addr);
	::pthread_mutex_unlock(&system_call_mutex);
	return addr_str;
}

int FSocket::getSinFamily() {
	return this->sin_family;
}

void FSocket::setSinFamily(int sin_family) {
	this->sin_family = sin_family;
}

int FSocket::setSocketTimeout(int send_flag, int timeout) {
	int optname = send_flag ? SO_SNDTIMEO : SO_RCVTIMEO;
	struct timeval time;
	time.tv_sec = timeout;
	time.tv_usec = 0;
	return ::setsockopt(this->sock_fd, SOL_SOCKET, optname, (char*) &time, sizeof(time));
}

} /* namespace freeyxm */

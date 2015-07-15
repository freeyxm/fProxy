/*
 * FProtocol.cpp
 *
 *  Created on: 2012-11-5
 *      Author: AKA
 */

#include "FProtocol.h"
#include "fcore/FUtil.h"
#include "fcore/FString.h"
#include <cstdio>
#include <cstdlib>
#ifdef __WIN32__
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

namespace freeyxm {

FProtocol::FProtocol() {
	// TODO Auto-generated constructor stub

}

FProtocol::~FProtocol() {
	// TODO Auto-generated destructor stub
}

int FProtocol::ip4_byte2str(const unsigned char *ip_byte, string &ip_str) {
	char buf[32];
	ip_str.clear();
	for (int i = 0; i < 4; ++i) {
		sprintf(buf, "%u%c", (unsigned int) ip_byte[i], '\0');
		if (!i) {
			ip_str += buf;
		} else {
			ip_str.append(".").append(buf);
		}
	}
	return 0;
}

int FProtocol::ip4_str2byte(const string ip_str, unsigned char *ip_byte) {
	size_t p1 = 0, p2;
	int ip[4];
	string ip1;
	for (int i = 0; i < 4; ++i) {
		p2 = ip_str.find('.', p1);
		if (p2 == string::npos) {
			if (i != 3) {
				return -1;
			} else {
				p2 = ip_str.length();
			}
		}
		ip1 = ip_str.substr(p1, p2 - p1);
		if (ip1.empty() || !FString::isAllDigit(ip1)) {
			return -1;
		}
		ip[i] = ::atoi(ip1.c_str());
		if (ip[i] < 0 || ip[i] > 255) {
			return -1;
		}
		ip_byte[i] = ip[i];
		p1 = p2 + 1;
	}
	return 0;
}

unsigned short FProtocol::ntohs(const unsigned char *p_ns) {
	return ::ntohs(*((unsigned short*) p_ns));
}

void FProtocol::htons(const unsigned short hs, char *p_ns) {
	*((unsigned short*) p_ns) = ::htons(hs);
}

void FProtocol::ntons(const unsigned short ns, char *p_ns) {
	*(unsigned short*) p_ns = ns;
}

/*
 * onceRecvAndSend
 * return: 1, success; 0, recv_socket closed; -1, error.
 */
int FProtocol::onceRecvAndSend(FSocketTcp *recv_socket, FSocketTcp *send_socket, char *buf, const int buf_size) {
	int nrecv, nsend;
	int ret = 1;
	do {
		nrecv = recv_socket->recv(buf, buf_size);
		if (nrecv < 0) {
			DEBUG_PRINTLN_ERR("recv error", recv_socket->getErrCode(), recv_socket->getErrStr().c_str());
			ret = -1;
			break;
		} else if (nrecv == 0) {
			ret = 0;
			break; // remote socket closed.
		}

		nsend = send_socket->send(buf, nrecv);
		if (nsend < 0 || nsend != nrecv) {
			DEBUG_PRINTLN_ERR("send error", send_socket->getErrCode(), send_socket->getErrStr().c_str());
			ret = -1;
			break;
		}
	} while (0);

	return ret;
}

int FProtocol::loopRecvAndSend(FSocketTcp *socket1, FSocketTcp *socket2) {
	int c_sid = socket1->getHandle();
	int s_sid = socket2->getHandle();
	fd_set r_fds;
	int nfds = c_sid > s_sid ? c_sid + 1 : s_sid + 1;
	int select_ret;

	char *relay_buf = new char[RELAY_BUF_SIZE + 1];
	if (!relay_buf) {
		DEBUG_PRINTLN_MSG("new relay_buf failed!");
		return -1;
	}

	while (1) {
		FD_ZERO(&r_fds);
		FD_SET(c_sid, &r_fds);
		FD_SET(s_sid, &r_fds);
		select_ret = ::select(nfds, &r_fds, NULL, NULL, NULL);
		if (select_ret < 0) {
			DEBUG_PRINTLN_ERR("select error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
			break;
		} else if (select_ret == 0) { // should't happen here!
			break;
		}

		if (select_ret && FD_ISSET(c_sid, &r_fds)) {
			if (FProtocol::onceRecvAndSend(socket1, socket2, relay_buf,
			RELAY_BUF_SIZE) <= 0) {
				break;
			}
			--select_ret;
		}
		if (select_ret && FD_ISSET(s_sid, &r_fds)) {
			if (FProtocol::onceRecvAndSend(socket2, socket1, relay_buf,
			RELAY_BUF_SIZE) <= 0) {
				break;
			}
			--select_ret;
		}
	} // end while

	if (relay_buf) {
		delete relay_buf;
	}

	return 0;
}

} /* namespace freeyxm */

/*
 * FPSocks5.cpp
 *
 *  Created on: 2012-11-6
 *      Author: AKA
 */

#include "FPSocks5.h"
#include "fcore/FUtil.h"
#include "fcore/FSocketUdp.h"
#include <string>
#include <cstring>
#include <algorithm>
#if __linux__
#include <sys/socket.h>
#endif

namespace freeyxm {

using std::find;

FP_Socks5::FP_Socks5() {
	// here to add validate method:
	method_list.push_back(S5_METHOD_UP);
	method_list.push_back(S5_METHOD_NONE);
}

FP_Socks5::~FP_Socks5() {
	// TODO Auto-generated destructor stub
}

int FP_Socks5::process(FSocketTcp *socket) {
	int ret = 0;

	do {
		if ((ret = dealValidate(socket)) <= 0) {
			break;
		}

		if ((ret = dealRequest(socket)) <= 0) {
			break;
		}
	} while (0);

	socket->close();

	return ret;
}

/*
 * dealValidate
 * return: -1, error; 0, success and complete; 1, success and need continue deal.
 */
int FP_Socks5::dealValidate(FSocketTcp *socket) {
	fp_socks5_method_t s5_method;

	int ret = socket->recv((char*) &s5_method, sizeof(s5_method));
	if (ret < 0) { // error occur.
		DEBUG_PRINTLN_ERR("recv error", socket->getErrCode(), socket->getErrStr().c_str());
		return -1;
	} else if (ret < S5_MIN_METHOD_SIZE) { // Bad request.
		DEBUG_PRINTLN_MSG("Bad request: method msg size smaller than S5_MIN_METHOD_SIZE!");
		return -1;
	}

	if (s5_method.ver != S5_VERSION || s5_method.nMethods < 1) { // Bad request.
		DEBUG_PRINTLN_MSG("Bad request: method msg field illegal!");
		return -1;
	}

	DEBUG_PRINT_LOCK();
	DEBUG_PRINT("ver:%d, nMethods: %d, methods:", s5_method.ver, s5_method.nMethods);
#ifdef _DEBUG_
	FUtil::print_hex(stdout,(unsigned char*) s5_method.methods,s5_method.nMethods,0);
#endif
	DEBUG_PRINT("\n");
	DEBUG_PRINT_UNLOCK();

	s5_method.ver = S5_VERSION;
	s5_method.method = this->choseMethod(&s5_method);
	int nsend = sizeof(s5_method.ver) + sizeof(s5_method.method);
	ret = socket->send((char*) &s5_method, nsend);
	if (ret < 0 || ret != nsend) { // send error.
		DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
		return -1;
	}

	if (s5_method.method == S5_METHOD_WRONG) { // NO ACCEPTABLE METHODS.
		return 0;
	} else {
		return this->validate(s5_method.method, socket);
	}

	return 1;
}

int FP_Socks5::choseMethod(const fp_socks5_method_t *method) {
	for (unsigned int i = 0; i < (unsigned int) method->nMethods; ++i) {
		if (find(method_list.begin(), method_list.end(), method->methods[i]) != method_list.end()) {
			return method->methods[i];
		}
	}
	return S5_METHOD_WRONG;
}

/*
 * validate
 * return: -1, error; 0, success and complete; 1, success and need continue deal.
 */
int FP_Socks5::validate(int method, FSocketTcp *socket) {
	switch (method) {
	case S5_METHOD_NONE:
		return 1;
		break;
	case S5_METHOD_UP: {
		fp_socks5_userpass_t s5_up, s5_up_reply;
		int ret = socket->recv((char*) &s5_up, S5_MAX_USERPASS_SIZE);
		if (ret < 0) {
			DEBUG_PRINTLN_ERR("recv error", socket->getErrCode(), socket->getErrStr().c_str());
			return -1;
		} else if (ret == 0) { // remote socket closed ...
			return 0;
		} else if (ret < S5_MIN_USERPASS_SIZE) {
			DEBUG_PRINTLN_MSG("Bad request: username/password msg size smaller than S5_MIN_USERPASS_SIZE!");
			return -1;
		}

		s5_up_reply.ver = S5_USERPASS_VERSION;
		s5_up_reply.status = S5_USERPASS_STATUS_SUCCESS;

		do {
			if (s5_up.ver != S5_USERPASS_VERSION) {
				s5_up_reply.status = S5_USERPASS_STATUS_FAILURE;
				DEBUG_PRINTLN_MSG("Bad request: username/password msg field illegal!");
				break;
			}

			// correct p_plen/p_password:
			s5_up.p_plen = &s5_up.ulen + 1 + s5_up.ulen;
			s5_up.p_password = s5_up.p_plen + 1;

			// get username/password:
			string username((char*) s5_up.username, s5_up.ulen);
			string password((char*) s5_up.p_password, *s5_up.p_plen);

			DEBUG_MPRINT("username: %s, password: %s\n", username.c_str(), password.c_str());

			// validate username/password ...
			if (!validateUserPass(username, password)) {
				s5_up_reply.status = S5_USERPASS_STATUS_FAILURE;
			}
		} while (0);

		int nsend = sizeof(s5_up_reply.ver) + sizeof(s5_up_reply.status);
		ret = socket->send((char*) &s5_up_reply, nsend);
		if (ret < 0 || ret != nsend) {
			DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
			return -1;
		}

		if (s5_up_reply.status == S5_USERPASS_STATUS_SUCCESS) {
			return 1;
		} else {
			return 0;
		}
	}
		break;
	default:
		return -1;
	} // end switch
	return 0;
}

int FP_Socks5::validateUserPass(const string &username, const string &password) {
	// ...
	if (username != "hithere" || password != "hereyouare") {
		return 0;
	}
	return 1;
}

int FP_Socks5::dealRequest(FSocketTcp *socket) {
	fp_socks5_request_t s5_request, s5_reply;

	int ret = socket->recv((char*) &s5_request, S5_MAX_REQUEST_SIZE);
	if (ret < 0) { // error.
		DEBUG_PRINTLN_ERR("recv error", socket->getErrCode(), socket->getErrStr().c_str());
		return -1;
	} else if (ret == 0) { // remote socket closed ...
		return 0;
	} else if (ret < S5_MIN_REQUEST_SIZE) { // Bad request.
		DEBUG_PRINTLN_MSG("Bad request: request msg size smaller than S5_MIN_REQUEST_SIZE!");
		return -1;
	}
	if (s5_request.ver != S5_VERSION || s5_request.rsv != S5_RESERVE) { // Bad request.
		DEBUG_PRINTLN_MSG("Bad request: request msg field illegal!");
		return -1;
	}

	DEBUG_PRINT_LOCK();
	DEBUG_PRINT("request: ");
#ifdef _DEBUG_
	FUtil::print_hex(stdout,(unsigned char*)&s5_request,ret,0);
#endif
	DEBUG_PRINT("\n");
	DEBUG_PRINT_UNLOCK();

	::memset(&s5_reply, 0, sizeof(s5_reply));
	s5_reply.ver = S5_VERSION;
	s5_reply.rsv = S5_RESERVE;
	s5_reply.rep = S5_REP_SUCCESS;
	string dst_addr;
	unsigned int dst_port;

	do {
		s5_reply.rep = this->parseAddrPort(&s5_request, ret, dst_addr, dst_port);
		if (s5_reply.rep != S5_REP_SUCCESS) {
			break;
		}

		switch (s5_request.cmd) {
		case S5_CMD_CONNECT:
			s5_reply.rep = this->dealRequestConnect(socket, &s5_request, dst_addr, dst_port);
			break;
		case S5_CMD_BIND:
			s5_reply.rep = this->dealRequestBind(socket, &s5_request, dst_addr, dst_port);
			break;
		case S5_CMD_UDP:
			s5_reply.rep = this->dealRequestUdp(socket, &s5_request, dst_addr, dst_port);
			break;
		default:
			s5_reply.rep = S5_REP_CMD_UNSUPPORTED;
			break;
		}
	} while (0);

	if (s5_reply.rep != S5_REP_SUCCESS && s5_reply.rep != S5_REP_TERMINATED) {
		int nsend = &s5_reply.rsv - &s5_reply.ver + sizeof(s5_reply.rsv); // byte after s5_repsly.rsv are all 0x00 now.
		ret = socket->send((char*) &s5_reply, nsend);
		if (ret < 0 || ret != nsend) {
			DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
			return -1;
		}
	}

	return 0;
}

s5_method_t FP_Socks5::parseAddrPort(fp_socks5_request_t *s5_request, const int request_size, string &dst_addr,
		unsigned int &dst_port) {
	s5_method_t rep = S5_REP_SUCCESS;

	// parse dst_addr:
	switch (s5_request->atyp) {
	case S5_ATYP_IP4:
		if (request_size != 10) {
			rep = S5_REP_FAILURE; // ...
			break;
		}
		if (this->ip4_byte2str(s5_request->addr_ip4, dst_addr) < 0) {
			rep = S5_REP_HOST_UNREACHABLE;
		}
		s5_request->p_port = s5_request->addr_ip4 + sizeof(s5_request->addr_ip4);
		break;
	case S5_ATYP_DOMAIN: {
		int len = (*(unsigned char*) s5_request->addr_domain);
		if (request_size != 6 + 1 + len) {
			rep = S5_REP_FAILURE; // ...
			break;
		}
		dst_addr.append((char*) (s5_request->addr_domain + 1), len);
		s5_request->p_port = s5_request->addr_domain + len + 1;
	}
		break;
	case S5_ATYP_IP6:
		if (request_size != 22) {
			rep = S5_REP_FAILURE; // ...
			break;
		}
		s5_request->p_port = s5_request->addr_ip4 + sizeof(s5_request->addr_ip6);
		rep = S5_REP_ATYP_UNSUPPORTED;
		break;
	default:
		rep = S5_REP_ATYP_UNSUPPORTED;
		break;
	}
	if (rep != S5_REP_SUCCESS) {
		return rep;
	}

	// parse dst_port:
	dst_port = this->ntohs(s5_request->p_port);

	DEBUG_MPRINT("request: addr: %s, port: %u\n", dst_addr.c_str(), dst_port);

	return rep;
}

s5_method_t FP_Socks5::parseAddrPort(fp_socks5_udp_t *s5_udp, int &udp_size, string &dst_addr, unsigned int &dst_port) {
	s5_method_t rep = S5_REP_SUCCESS;

	// parse dst_addr:
	switch (s5_udp->atyp) {
	case S5_ATYP_IP4:
		if (udp_size <= 10) {
			rep = S5_REP_FAILURE; // ...
			break;
		}
		udp_size -= 10;
		if (this->ip4_byte2str(s5_udp->addr_ip4, dst_addr) < 0) {
			rep = S5_REP_HOST_UNREACHABLE;
		}
		s5_udp->p_port = s5_udp->addr_ip4 + sizeof(s5_udp->addr_ip4);
		break;
	case S5_ATYP_DOMAIN: {
		int len = (*(unsigned char*) s5_udp->addr_domain);
		if (udp_size <= 6 + 1 + len) {
			rep = S5_REP_FAILURE; // ...
			break;
		}
		udp_size -= 6 + 1 + len;
		dst_addr.append((char*) (s5_udp->addr_domain + 1), len);
		s5_udp->p_port = s5_udp->addr_domain + len + 1;
	}
		break;
	case S5_ATYP_IP6:
		if (udp_size <= 22) {
			rep = S5_REP_FAILURE; // ...
			break;
		}
		udp_size -= 22;
		s5_udp->p_port = s5_udp->addr_ip4 + sizeof(s5_udp->addr_ip6);
		rep = S5_REP_ATYP_UNSUPPORTED;
		break;
	default:
		rep = S5_REP_ATYP_UNSUPPORTED;
		break;
	}
	if (rep != S5_REP_SUCCESS) {
		return rep;
	}
	s5_udp->p_data = s5_udp->p_port + 2;

	// parse dst_port:
	dst_port = this->ntohs(s5_udp->p_port);

	DEBUG_MPRINT("request: addr: %s, port: %u\n", dst_addr.c_str(), dst_port);

	return rep;
}

/*
 * dealReauestConnect.
 * return:
 *   S5_REP_SUCCESS: end success.
 *   S5_REP_TERMINATED: end with error, but don't need reply.
 *   other: end with error, need to reply.
 */
s5_method_t FP_Socks5::dealRequestConnect(FSocketTcp *socket, const fp_socks5_request_t *s5_request, const string dst_addr,
		const unsigned int dst_port) {
	s5_method_t rep = S5_REP_SUCCESS;
	FSocketTcp serv_socket;

	do {
		if (serv_socket.connect(dst_addr.c_str(), dst_port) < 0) {
			DEBUG_PRINTLN_ERR("connect error", socket->getErrCode(), socket->getErrStr().c_str());
			rep = S5_REP_HOST_UNREACHABLE;
			break;
		}

		// request reply:
		fp_socks5_request_t s5_reply;
		s5_reply.ver = S5_VERSION;
		s5_reply.rep = S5_REP_SUCCESS;
		s5_reply.rsv = S5_RESERVE;
		int nsend = 4 + 2; // ...
		switch (s5_request->atyp) { // dst_addr bytes.
		case S5_ATYP_IP4:
			nsend += 4;
			break;
		case S5_ATYP_IP6:
			nsend += 16;
			break;
		case S5_ATYP_DOMAIN:
			nsend += (unsigned int) s5_request->addr_domain[0] + 1;
			break;
		default: // this should't happen here.
			break;
		}
		memcpy(&s5_reply.atyp, &s5_request->atyp, nsend - 6);

		int ret = socket->send((char*) &s5_reply, nsend);
		if (ret < 0 || ret != nsend) {
			DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
			rep = S5_REP_TERMINATED;
			break;
		}

		FProtocol::loopRecvAndSend(socket, &serv_socket);
		serv_socket.close();
	} while (0);

	return rep;
}

/*
 * dealReauestBind.
 * return:
 *   S5_REP_SUCCESS: end success.
 *   S5_REP_TERMINATED: end with error, but don't need reply.
 *   other: end with error, need to reply.
 */
s5_method_t FP_Socks5::dealRequestBind(FSocketTcp *socket, const fp_socks5_request_t *s5_request, const string dst_addr,
		const unsigned int dst_port) {

	// here should check whether client has a good connect with server or not use dst_addr/dst_port!

	FSocketTcp bind_socket;
	FSocketTcp *server_socket = NULL;
	int ret;
	// bind socket:
	do {
		ret = bind_socket.bind(NULL, 0);
		if (ret < 0) {
			DEBUG_PRINTLN_ERR("bind error", bind_socket.getErrCode(), bind_socket.getErrStr().c_str());
			return S5_REP_FAILURE;
		}
		{
			sockaddr_in addr;
			unsigned int addr_len = sizeof(addr);
			ret = ::getsockname(bind_socket.getHandle(), (struct sockaddr*) &addr, &addr_len);
			if (ret < 0) {
				DEBUG_PRINTLN_ERR("getsockname error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
				return S5_REP_FAILURE;
			}
			bind_socket.setLocalAddress(addr);
		}

		if (bind_socket.listen()) {
			DEBUG_PRINTLN_ERR("listen error", bind_socket.getErrCode(), bind_socket.getErrStr().c_str());
			return S5_REP_FAILURE;
		}

		// first bind reply:
		fp_socks5_request_t bind_reply;
		bind_reply.ver = S5_VERSION;
		bind_reply.rep = S5_REP_SUCCESS;
		bind_reply.rsv = S5_RESERVE;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		bind_reply.atyp = S5_ATYP_IP4; // chang to support ip6 ... !!!
		string bind_addr = FSocket::inet_ntoa(bind_socket.getLocalAddress().sin_addr);
		FProtocol::ip4_str2byte(bind_addr.c_str(), bind_reply.addr_ip4);
		FProtocol::ntons(bind_socket.getLocalAddress().sin_port, (char*) bind_reply.addr_ip4 + 4);
		int nsend = 4 + 4 + 2;
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		ret = socket->send((char*) &bind_reply, nsend);
		if (ret < 0 || ret != nsend) {
			DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
			return S5_REP_TERMINATED;
		}

		do {
			// If a rancorous client close socket just after send a bind request,
			// socks server may wait here forever, so It's important to find a solution !!!
			server_socket = bind_socket.accept();
			if (!server_socket) {
				DEBUG_PRINTLN_ERR("accept error", server_socket->getErrCode(), server_socket->getErrStr().c_str());
				return S5_REP_FAILURE;
			}
			// check addr == dst_addr:
			string req_addr = FSocket::inet_ntoa(bind_socket.getRemoteAddress().sin_addr);
			if (req_addr != dst_addr) {
				DEBUG_MPRINT("addr validate failed, dst_addr: %s, req_addr: %s.\n", dst_addr.c_str(), req_addr.c_str());
				delete server_socket; // auto close().
				continue;
			}

			// second bind reply:
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			FProtocol::ip4_str2byte(bind_addr, bind_reply.addr_ip4); // ... ???
			unsigned short bind_port = bind_socket.getLocalAddress().sin_port; // ... ???
			::memcpy(bind_reply.addr_ip4 + 4, &bind_port, 2);
			nsend = 4 + 4 + 2;
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			ret = socket->send((char*) &bind_reply, nsend);
			if (ret < 0 || ret != nsend) {
				DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
				delete server_socket; // auto close().
				return S5_REP_TERMINATED;
			}
		} while (0);

		FProtocol::loopRecvAndSend(socket, server_socket);
		bind_socket.close();
		delete server_socket; // auto close().
	} while (0);

	return S5_REP_SUCCESS;
}

/*
 * dealReauestUdp.
 * return:
 *   S5_REP_SUCCESS: end success.
 *   S5_REP_TERMINATED: end with error, but don't need reply.
 *   other: end with error, need to reply.
 */
s5_method_t FP_Socks5::dealRequestUdp(FSocketTcp *socket, const fp_socks5_request_t *s5_request, const string src_addr,
		const unsigned int src_port) {

	FSocketUdp udp_serv_socket, udp_socket;
	int ret;
	do {
		ret = udp_serv_socket.bind(NULL, 0); // ...
		if (ret < 0) {
			DEBUG_PRINTLN_ERR("bind error", udp_serv_socket.getErrCode(), udp_serv_socket.getErrStr().c_str());
			return S5_REP_FAILURE;
		}
		{
			sockaddr_in addr;
			unsigned int addr_len = sizeof(addr);
			ret = ::getsockname(udp_serv_socket.getHandle(), (struct sockaddr*) &addr, &addr_len);
			if (ret < 0) {
				DEBUG_PRINTLN_ERR("getsockname error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
				return S5_REP_FAILURE;
			}
			udp_serv_socket.setLocalAddress(addr);
		}
		if (udp_socket.createSocket()) {
			DEBUG_PRINTLN_ERR("socket error", udp_serv_socket.getErrCode(), udp_serv_socket.getErrStr().c_str());
			return S5_REP_FAILURE;
		}
	} while (0);

	fp_socks5_request_t bind_reply;
	bind_reply.ver = S5_VERSION;
	bind_reply.rep = S5_REP_SUCCESS;
	bind_reply.rsv = S5_RESERVE;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	bind_reply.atyp = S5_ATYP_IP4;
	bind_reply.p_port = bind_reply.addr_ip4 + 4;
	{
		sockaddr_in addr = udp_serv_socket.getLocalAddress();
		//string bind_addr = FSocket::inet_ntoa(addr.sin_addr); // how to get my ip which connected by client ???
		string bind_addr = "127.0.0.1";
		FProtocol::ip4_str2byte(bind_addr.c_str(), bind_reply.addr_ip4);
		FProtocol::ntons(addr.sin_port, (char*) bind_reply.p_port);
	}
	const int nsend = 4 + 4 + 2;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	ret = socket->send((char*) &bind_reply, nsend);
	if (ret < 0 || ret != nsend) {
		DEBUG_PRINTLN_ERR("send error", socket->getErrCode(), socket->getErrStr().c_str());
		return S5_REP_TERMINATED;
	}

	fp_socks5_udp_t s5_udp;
	sockaddr_in client_addr;
	::memset(&client_addr, 0, sizeof(client_addr));
	string dst_addr;
	unsigned int dst_port = 0;
	int nrecv, select_ret;

	int udp_serv_sid = udp_serv_socket.getHandle();
	int udp_sid = udp_socket.getHandle();
	int tcp_sid = socket->getHandle();
	fd_set r_fds;
	int nfds = udp_serv_sid > udp_sid ? udp_serv_sid + 1 : udp_sid + 1;
	nfds = nfds > tcp_sid ? nfds : tcp_sid + 1;
	const int is_check_addr = (src_addr != "0.0.0.0");

	while (1) {
		FD_ZERO(&r_fds);
		FD_SET(udp_serv_sid, &r_fds);
		FD_SET(udp_sid, &r_fds);
		FD_SET(tcp_sid, &r_fds);
		select_ret = ::select(nfds, &r_fds, NULL, NULL, NULL);
		if (select_ret < 0) {
			DEBUG_PRINTLN_ERR("select error", FUtil::getErrCode(), FUtil::getErrStr().c_str());
			break;
		} else if (select_ret == 0) { // should't happen here!
			break;
		}

		if (select_ret && FD_ISSET(udp_serv_sid, &r_fds)) {
			do {
				ret = 0;
				nrecv = udp_serv_socket.recvFrom((char*) &s5_udp,
				S5_MAX_UDP_SIZE, &client_addr);
				if (nrecv < 0) {
					DEBUG_PRINTLN_ERR("recvfrom error", udp_serv_socket.getErrCode(), udp_serv_socket.getErrStr().c_str());
					ret = -1;
					break;
				} else if (nrecv == 0) { // the peer has performed an orderly shutdown.
					ret = 1; // end and break;
					break;
				}

				if (nrecv <= S5_MIN_UDP_SIZE || s5_udp.frag // do not support frag.
						|| s5_udp.rsv[0] != S5_UDP_RESERVED1 || s5_udp.rsv[1] != S5_UDP_RESERVED2) { // illegal msg.
					break;
				}
				if (src_port && (src_port != ::ntohs(client_addr.sin_port))) {
					break;
				}
				if (is_check_addr && (src_addr != FSocket::inet_ntoa(client_addr.sin_addr))) {
					break;
				}
				if (this->parseAddrPort(&s5_udp, nrecv, dst_addr, dst_port) != S5_REP_SUCCESS) {
					break;
				}

				ret = udp_socket.sendTo((char*) s5_udp.p_data, nrecv, dst_addr.c_str(), dst_port); // nrecv updated by parseAddrPort.
				if (ret < 0 || ret != nrecv) {
					DEBUG_PRINTLN_ERR("sendto error", udp_socket.getErrCode(), udp_socket.getErrStr().c_str());
					ret = -1; // error and break;
					break;
				}

				DEBUG_MPRINT("client, nrecv= %d, sendto= %s:%u\n", nrecv, dst_addr.c_str(), dst_port);
			} while (0);
			if (ret) {
				break;
			}
			--select_ret;
		} // end if
		if (select_ret && FD_ISSET(udp_sid, &r_fds)) {
			nrecv = udp_socket.recvFrom((char*) &s5_udp, S5_MAX_UDP_SIZE, NULL);
			DEBUG_MPRINT("server, nrecv= %d\n", nrecv);
			if (nrecv < 0) {
				DEBUG_PRINTLN_ERR("recvfrom error", udp_socket.getErrCode(), udp_socket.getErrStr().c_str());
				break;
			} else if (nrecv == 0) { // the peer has performed an orderly shutdown.
				break;
			}
			if (!client_addr.sin_port) {
				break;
			}

			ret = udp_serv_socket.sendTo((char*) &s5_udp, nrecv, &client_addr);
			if (ret < 0 || ret != nrecv) {
				DEBUG_PRINTLN_ERR("sendto error", udp_serv_socket.getErrCode(), udp_serv_socket.getErrStr().c_str());
				break;
			}
			--select_ret;
		} // end if
		if (select_ret && FD_ISSET(tcp_sid, &r_fds)) { // A UDP association terminates when the TCP connection that the UDP ASSOCIATE request arrived on terminates.
			ret = socket->recv((char*) &s5_udp, S5_MAX_UDP_SIZE);
			if (ret == 0) {
				break;
			}
		} // end if
	} // end while

	udp_serv_socket.close();
	udp_socket.close();

	return S5_REP_SUCCESS;
}

} /* namespace freeyxm */

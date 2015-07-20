/*
 * FPSocks52.cpp
 *
 *  Created on: 2015年7月16日
 *      Author: aka
 */

#include <fprotocol/FPSocks52.h>
#include <fprotocol/FProtocol.h>
#include <cstring>
#if __linux__
#include <arpa/inet.h>
#endif

namespace freeyxm {

namespace socks5 {

using namespace freeyxm;

FP_Socks5_2::FP_Socks5_2(FSocketTcp *socket) :
		m_pSocket(socket), m_state(State_New)
{
	initMethods();
}

FP_Socks5_2::~FP_Socks5_2()
{
	// TODO Auto-generated destructor stub
}

void FP_Socks5_2::initMethods()
{
	m_methodNum = 2; // ...
	m_pMethods = new method_info[m_methodNum];

	m_pMethods[0].method = Method::METHOD_UN_PW;
	m_pMethods[0].fun = &FP_Socks5_2::authUnPw;

	m_pMethods[1].method = Method::METHOD_None;
	m_pMethods[1].fun = &FP_Socks5_2::authNone;
}

int FP_Socks5_2::run()
{
	int ret = 0;

	m_state = State_Select_Method;

	bool running = true;
	while (running)
	{
		switch (m_state)
		{
			case State_Select_Method:
			{
				ret = methodSelect();
				if (ret == 0)
					m_state = State_Auth;
				else
					m_state = State_Close;
			}
				break;
			case State_Auth:
			{
				ret = auth();
				if (ret == 0)
					m_state = State_Request;
				else
					m_state = State_Close;
			}
				break;
			case State_Request:
			{
				ret = doRequest();
				// ...
			}
				break;
			case State_Close:
			{
				close();
				m_state = State_Closed;
			}
				break;
			default:
				running = false;
				break;
		}
	}

	return ret;
}

void FP_Socks5_2::close()
{
	if (m_pSocket)
	{
		m_pSocket->close();
		m_pSocket = NULL;
	}
}

int FP_Socks5_2::methodSelect()
{
	method_request_t request;
	int nrecv = m_pSocket->recv((char*) &request, sizeof(request));
	if (nrecv < 0)
	{
		DEBUG_PRINTLN_ERR("recv error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}
	else if (nrecv < METHOD_REQUEST_MIN_LEN)
	{
		DEBUG_PRINTLN_MSG("Bad request: method_request len < METHOD_REQUEST_MIN_LEN!");
		return -1;
	}

	if (request.ver != S5_VERSION)
	{
		DEBUG_PRINTLN_MSG("Bad request: method_request ver illegal!");
		return -1;
	}

	method_reply_t reply;
	reply.ver = S5_VERSION;
	reply.method = methodSelect(request);
	m_method = reply.method;

	int nsend = m_pSocket->send((const char*) &reply, sizeof(reply));
	if (nsend < 0 || nsend != sizeof(reply))
	{
		DEBUG_PRINTLN_ERR("send error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}

	return reply.method == Method::METHOD_WRONG ? 1 : 0;
}

int FP_Socks5_2::methodSelect(const method_request_t& request)
{
	for (int i = 0; i < m_methodNum; ++i)
	{
		Byte method = m_pMethods[i].method;
		for (int k = 0; k < request.nmethods; ++k)
		{
			if (method == request.methods[k])
				return method;
		}
	}
	return Method::METHOD_WRONG;
}

int FP_Socks5_2::auth()
{
	switch (m_method)
	{
		case Method::METHOD_None:
			return authNone();
		case Method::METHOD_UN_PW:
			return authUnPw();
		default:
			return -1;
	}
}

int FP_Socks5_2::authNone()
{
	return 0;
}

int FP_Socks5_2::authUnPw()
{
	method_unpw_request_t request;
	int nrecv = m_pSocket->recv((char*) &request, sizeof(request));
	if (nrecv < 0)
	{
		DEBUG_PRINTLN_ERR("recv error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}
	else if (nrecv < METHOD_UNPW_REQUEST_MIN_LEN)
	{
		DEBUG_PRINTLN_MSG("Bad request: method_unpw_request len < METHOD_UNPW_REQUEST_MIN_LEN!");
		return -1;
	}

	if (request.ver != S5_SUB_NEGOTIATION_VER)
	{
		DEBUG_PRINTLN_MSG("Bad request: method_unpw_request ver illegal!");
		return -1;
	}

	method_unpw_reply_t reply;
	reply.ver = S5_SUB_NEGOTIATION_VER;
	reply.status = authUnPw(request);

	int nsend = m_pSocket->send((const char*) &reply, sizeof(reply));
	if (nsend < 0 || nsend != sizeof(reply))
	{
		DEBUG_PRINTLN_ERR("send error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}

	return reply.status;
}

int FP_Socks5_2::authUnPw(const method_unpw_request_t &request)
{
	static char username[] = "aka"; // ...
	static char password[] = "justme"; // ...

	if (request.ulen != sizeof(username) || request.plen != sizeof(password))
		return -1;

	bool pass = (::strncmp((const char*) &request.username, username, request.ulen) == 0
			&& ::strncmp((const char*) &request.password, password, request.plen) == 0);
	return pass ? 0 : -1;
}

int FP_Socks5_2::doRequest()
{
	request_detail_t request;
	int nrecv = m_pSocket->recv((char*) &request, sizeof(request));
	if (nrecv < 0)
	{
		DEBUG_PRINTLN_ERR("recv error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}
	else if (nrecv == 0)
	{
		m_state = State_Close;
		return 0;
	}
	else if (nrecv < REQUEST_DETAIL_MIN_LEN)
	{
		DEBUG_PRINTLN_MSG("Bad request: request_detail len < REQUEST_DETAIL_MIN_LEN!");
		return -1;
	}

	if (request.ver != S5_VERSION || request.rsv != S5_RESERVE)
	{
		DEBUG_PRINTLN_MSG("Bad request: request_detail ver illegal!");
		return -1;
	}

	return doRequest(request, nrecv);
}

int FP_Socks5_2::doRequest(const request_detail_t& request, int nrecv)
{
	Address address;
	int ret = parseAddrPort(request.address, nrecv - (sizeof(request) - sizeof(request.address)), address);
	if (ret != ReplyCode::REPLY_SUCCESS)
	{
		request_reply_t reply;
		reply.ver = S5_VERSION;
		reply.rep = ret;
		reply.rsv = S5_RESERVE;
		sendReply(reply, 3);
		return -1;
	}

	DEBUG_PRINT_T("request: cmd = %u, addr = %s, port = %d\n", request.cmd, address.addr.c_str(), address.port);

	switch (request.cmd)
	{
		case CmdType::CMD_CONNECT:
			return doRequestConnect(address);
			break;
		case CmdType::CMD_BIND:
			return doRequestBind(address);
			break;
		case CmdType::CMD_UDP_ASSOCIATE:
			return doRequestUdpAssociate(address);
			break;
		default:
			DEBUG_PRINTLN_MSG("Bad request: request_detail cmd illegal!")
			;
			return -1;
	}
}

int FP_Socks5_2::doRequestConnect(const Address &address)
{
	int rep = ReplyCode::REPLY_FAILURE;
	FSocketTcp *serv_socket = new FSocketTcp();

	do
	{
		if (serv_socket->connect(address.addr.c_str(), address.port) < 0)
		{
			DEBUG_PRINTLN_ERR("connect error", serv_socket->getErrCode(), serv_socket->getErrStr().c_str());
			rep = ReplyCode::REPLY_HOST_UNREACHABLE;
			break;
		}

		// request reply:
		request_reply_t reply;
		reply.ver = S5_VERSION;
		reply.rep = ReplyCode::REPLY_SUCCESS;
		reply.rsv = S5_RESERVE;
		reply.address.atyp = AddrType::ADDR_IPV4; // need to repair!!!

		int nsend = sizeof(reply) - sizeof(reply.address) + sizeof(reply.address.atyp);
		switch (reply.address.atyp)
		{ // dst_addr bytes.
			case AddrType::ADDR_IPV4:
			{
				sockaddr_in bind_addr = serv_socket->getLocalAddress();
				::memcpy(&reply.address.addr + 0, &bind_addr.sin_addr, 4);
				::memcpy(&reply.address.addr + 4, &bind_addr.sin_port, 2);
				nsend += 6;
			}
				break;
			default: // this should't happen here.
				break;
		}

		int ret = m_pSocket->send((char*) &reply, nsend);
		if (ret < 0 || ret != nsend)
		{
			DEBUG_PRINTLN_ERR("send error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
			rep = ReplyCode::REPLY_TERMINATE;
			break;
		}

		FProtocol::loopRecvAndSend(m_pSocket, serv_socket);
		serv_socket->close();

	} while (0);

	delete serv_socket;

	return rep;
}

int FP_Socks5_2::doRequestBind(const Address &address)
{
	return 0;
}

int FP_Socks5_2::doRequestUdpAssociate(const Address &address)
{
	return 0;
}

int FP_Socks5_2::sendReply(const request_reply_t& reply, unsigned int size)
{
	int nsend = m_pSocket->send((const char*) &reply, size);
	if (nsend < 0 || nsend != (int) size)
	{
		DEBUG_PRINTLN_ERR("send error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}
	return 0;
}

int FP_Socks5_2::parseAddrPort(const address_t& in_address, int nrecv, Address &out_address)
{
	out_address.atyp = in_address.atyp;
	out_address.addr.clear();

	bool len_match = false;
	int portOffset = 0;

	switch (in_address.atyp)
	{
		case AddrType::ADDR_IPV4:
		{
			len_match = (nrecv == 7); // 1 + 4 + 2
			if (!len_match)
				break;
			char buf[16];
			::inet_ntop(AF_INET, in_address.addr, buf, sizeof(buf));
			out_address.addr.append((buf));
			portOffset = 4;
		}
			break;
		case AddrType::ADDR_DOMAIN_NAME:
		{
			Byte len = in_address.addr[0];
			len_match = (nrecv == len + 4); // 1 + 1 + len + 2
			if (!len_match)
				break;
			out_address.addr.append((char*) &in_address.addr[1], len);
			portOffset = len + 1;
		}
			break;
		case AddrType::ADDR_IPV6:
		{
			len_match = (nrecv == 19); // 1 + 16 + 2
			if (!len_match)
				break;
			char buf[40];
			::inet_ntop(AF_INET6, in_address.addr, buf, sizeof(buf));
			out_address.addr.append(buf);
			portOffset = 16;
		}
			break;
		default:
			DEBUG_PRINTLN_MSG("Bad request: request_detail atyp illegal!")
			;
			return ReplyCode::REPLY_FAILURE;
			break;
	}

	if (!len_match)
	{
		DEBUG_PRINTLN_MSG("Bad request: request_detail len mismatch!");
		return ReplyCode::REPLY_FAILURE;
	}

	uint16_t pp = *(uint16_t*) (in_address.addr + portOffset);
	out_address.port = ::ntohs(pp);

	return ReplyCode::REPLY_SUCCESS;
}

} /* namespace socks5 */
} /* namespace freeyxm */

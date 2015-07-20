/*
 * FPSocks52.cpp
 *
 *  Created on: 2015年7月16日
 *      Author: aka
 */

#include <fprotocol/FPSocks52.h>
#include <fprotocol/FProtocol.h>
#include <cstring>
#include <tr1/memory>
#if __linux__
#include <arpa/inet.h>
#endif

namespace freeyxm {

namespace socks5 {

using namespace freeyxm;

FP_Socks5_2::FP_Socks5_2(FSocketTcp *socket) :
		m_pSocket(socket), m_state(State_New)
{
	initAuthMethods();
}

FP_Socks5_2::~FP_Socks5_2()
{
	// m_pSocket managed by caller.
}

void FP_Socks5_2::initAuthMethods()
{
	AddAuthMethod(MethodType::METHOD_UN_PW, &FP_Socks5_2::authUnPw);
	AddAuthMethod(MethodType::METHOD_None, &FP_Socks5_2::authNone);
}

void FP_Socks5_2::AddAuthMethod(Byte method, auth_method_fun fun)
{
	MethodInfo methodInfo = { method, fun };
	m_authMethods.push_back(methodInfo);
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
				m_state = State_Close;
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
	m_authMethod = reply.method;

	int nsend = m_pSocket->send((const char*) &reply, sizeof(reply));
	if (nsend < 0 || nsend != sizeof(reply))
	{
		DEBUG_PRINTLN_ERR("send error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
		return -1;
	}

	return reply.method == MethodType::METHOD_WRONG ? 1 : 0;
}

int FP_Socks5_2::methodSelect(const method_request_t& request)
{
	for (std::list<MethodInfo>::iterator it = m_authMethods.begin(); it != m_authMethods.end(); ++it)
	{
		for (int k = 0; k < request.nmethods; ++k)
		{
			if (it->method == request.methods[k])
				return it->method;
		}
	}
	return MethodType::METHOD_WRONG;
}

int FP_Socks5_2::auth()
{
	switch (m_authMethod)
	{
		case MethodType::METHOD_None:
			return authNone();
		case MethodType::METHOD_UN_PW:
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
		sendRequestReply(ret);
		return -1;
	}

	DEBUG_PRINT_T("request: cmd = %u, addr = %s, port = %d\n", request.cmd, address.addr.c_str(), address.port);

	int rep = ReplyCode::REPLY_SUCCESS;
	switch (request.cmd)
	{
		case CmdType::CMD_CONNECT:
			rep = doRequestConnect(address);
			break;
		case CmdType::CMD_BIND:
			rep = doRequestBind(address);
			break;
		case CmdType::CMD_UDP_ASSOCIATE:
			rep = doRequestUdpAssociate(address);
			break;
		default:
			rep = ReplyCode::REPLY_CMD_NOT_SUPPORTED;
			DEBUG_PRINTLN_MSG("Bad request: request_detail cmd illegal!")
			;
			return -1;
	}

	if (rep != ReplyCode::REPLY_SUCCESS && rep != ReplyCode::REPLY_TERMINATE)
	{
		sendRequestReply(rep);
	}

	return rep == ReplyCode::REPLY_SUCCESS ? 0 : -1;
}

int FP_Socks5_2::doRequestConnect(const Address &address)
{
	int rep = ReplyCode::REPLY_SUCCESS;
	std::tr1::shared_ptr<FSocketTcp> serv_socket(new FSocketTcp());

	do
	{
		if (serv_socket->connect(address.addr.c_str(), address.port) < 0)
		{
			rep = ReplyCode::REPLY_HOST_UNREACHABLE;
			DEBUG_PRINTLN_ERR("connect error", serv_socket->getErrCode(), serv_socket->getErrStr().c_str());
			break;
		}

		// request reply:
		request_reply_t reply;
		reply.ver = S5_VERSION;
		reply.rep = ReplyCode::REPLY_SUCCESS;
		reply.rsv = S5_RESERVE;
		reply.address.atyp = AddrType::ADDR_IPV4; // need to repair!!!

		int sendNum = sizeof(reply) - sizeof(reply.address) + sizeof(reply.address.atyp);
		switch (reply.address.atyp)
		{ // dst_addr bytes.
			case AddrType::ADDR_IPV4:
			{
				sockaddr_in bind_addr = serv_socket->getLocalAddress();
				::memcpy(&reply.address.addr + 0, &bind_addr.sin_addr, 4);
				::memcpy(&reply.address.addr + 4, &bind_addr.sin_port, 2);
				sendNum += 6;
			}
				break;
			default:
				rep = ReplyCode::REPLY_ATYP_NOT_SUPPORTED;
				break;
		}
		if (rep != ReplyCode::REPLY_SUCCESS)
			break;

		int nsend = m_pSocket->send((char*) &reply, sendNum);
		if (nsend < 0 || nsend != sendNum)
		{
			rep = ReplyCode::REPLY_TERMINATE;
			DEBUG_PRINTLN_ERR("send error", m_pSocket->getErrCode(), m_pSocket->getErrStr().c_str());
			break;
		}

		FProtocol::loopRecvAndSend(m_pSocket, serv_socket.get());
		serv_socket->close();

	} while (0);

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

int FP_Socks5_2::sendRequestReply(Byte replyCode)
{
	request_reply_t reply;
	reply.ver = S5_VERSION;
	reply.rep = replyCode;
	reply.rsv = S5_RESERVE;

	int nsend = m_pSocket->send((const char*) &reply, 3);
	if (nsend < 0 || nsend != 3)
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

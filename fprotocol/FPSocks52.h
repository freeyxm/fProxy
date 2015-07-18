/*
 * FPSocks52.h
 *
 *  Created on: 2015年7月16日
 *      Author: aka
 */

#ifndef FPROTOCOL_FPSOCKS52_H_
#define FPROTOCOL_FPSOCKS52_H_

#include <fcore/FCore.h>
#include <fcore/FSocketTcp.h>
#include <fcore/FSocketUdp.h>
#include <list>

namespace freeyxm {

namespace socks5 {

typedef unsigned char Byte;

//====================================================================
typedef struct {
	Byte ver;
	Byte nmethods;
	Byte methods[255];
} method_request_t;

typedef struct {
	Byte ver;
	Byte method;
} method_reply_t;

typedef struct {
	Byte ver;
	Byte ulen;
	Byte username[255];
	Byte plen;
	Byte password[255];
} method_unpw_request_t;

typedef struct {
	Byte ver;
	Byte status;
} method_unpw_reply_t;

typedef struct {
	Byte atyp;
	Byte addr[256];
	Byte port[2];
} address_t;

class Address {
public:
	Byte atyp;
	string addr;
	int port;
};

typedef struct {
	Byte ver;
	Byte cmd;
	Byte rsv;
	address_t address;
} request_detail_t;

typedef struct {
	Byte ver;
	Byte rep;
	Byte rsv;
	address_t address;
} request_reply_t;

//====================================================================
struct Method {
	const static int METHOD_None = 0x00; // X'00' NO AUTHENTICATION REQUIRED
	const static int METHOD_GSSAPI = 0x01; // X'01' GSSAPI
	const static int METHOD_UN_PW = 0X02; // X'02' USERNAME/PASSWORD
	const static int METHOD_IANA_MIN = 0X03; // X'03' to X'7F' IANA ASSIGNED
	const static int METHOD_IANA_MAX = 0X7F; // X'03' to X'7F' IANA ASSIGNED
	const static int METHOD_PRIVATE_MIN = 0X80; // X'80' to X'FE' RESERVED FOR PRIVATE METHODS
	const static int METHOD_PRIVATE_MAX = 0XFE; // X'80' to X'FE' RESERVED FOR PRIVATE METHODS
	const static int METHOD_WRONG = 0XFF; // X'FF' NO ACCEPTABLE METHODS
};

struct CmdType {
	const static int CMD_CONNECT = 0x01;
	const static int CMD_BIND = 0x02;
	const static int CMD_UDP_ASSOCIATE = 0x03;
};

struct AddrType {
	const static int ADDR_IPV4 = 0x01; // 4 octets
	const static int ADDR_DOMAIN_NAME = 0X03; // 1 + (0:255) octets
	const static int ADDR_IPV6 = 0x04; // 16 octets
};

struct ReplyCode {
	const static int REPLY_SUCCESS = 0x00; // succeeded
	const static int REPLY_FAILURE = 0x01; // general SOCKS server failure
	const static int REPLY_RULESET = 0x02; // connection not allowed by ruleset
	const static int REPLY_NET_UNREACHABLE = 0x03; // Network unreachable
	const static int REPLY_HOST_UNREACHABLE = 0x04; // Host unreachable
	const static int REPLY_REFUSED = 0x05; // Connection refused
	const static int REPLY_TTL_EXPIRED = 0x06; // TTL expired
	const static int REPLY_CMD_NOT_SUPPORTED = 0x07; // Command not supported
	const static int REPLY_ATYP_NOT_SUPPORTED = 0x08; // Address type not supported
	const static int REPLY_UNASSIGNED_MIN = 0x09; // X'09' to X'FF' unassigned
	const static int REPLY_UNASSIGNED_MAX = 0xFF; // X'09' to X'FF' unassigned
};

//====================================================================
#define S5_VERSION      0x05 // VERSION
#define S5_RESERVE      0x00 // RESERVED
#define S5_SUB_NEGOTIATION_VER 0x01

#define METHOD_REQUEST_MIN_LEN 3
#define METHOD_UNPW_REQUEST_MIN_LEN 3
#define REQUEST_DETAIL_MIN_LEN 10

//====================================================================
class FP_Socks5_2;

typedef int (FP_Socks5_2::*auth_method_fun)(void);
typedef struct {
	Byte method;
	auth_method_fun fun;
} method_info;

typedef enum {
	State_New,
	State_Select_Method,
	State_Auth,
	State_Request,
	State_Close,
	State_Closed,
} socks_state;

//====================================================================

class FP_Socks5_2 {
public:
	FP_Socks5_2(FSocketTcp *socket);
	virtual ~FP_Socks5_2();

	int run();

protected:
	int methodSelect();
	int methodSelect(const method_request_t& request);

	int auth();
	int authUnPw();
	int authUnPw(const method_unpw_request_t& request);

	int doRequest();
	int doRequest(const request_detail_t& request, int nrecv);
	int doRequestConnect(const Address &address);
	int doRequestBind(const Address &address);
	int doRequestUdpAssociate(const Address &address);

	int sendReply(const request_reply_t& reply, unsigned int size);

	int parseAddrPort(const address_t& in_address, int nrecv, Address& out_address);

	void close();

	virtual void initMethods();

	typedef struct {
		std::list<FSocketTcp*> m_tcpSockets;
		std::list<FSocketUdp*> m_udpSockets;
	} TaskInfo;

private:
	FSocketTcp *m_pSocket;
	socks_state m_state;

	int m_methodNum; // auth methods num.
	method_info *m_pMethods; // auth methods.
	Byte m_method; // selected method for auth.

	std::list<TaskInfo> m_tasks;
};

} /* namespace socks5 */
} /* namespace freeyxm */

#endif /* FPROTOCOL_FPSOCKS52_H_ */

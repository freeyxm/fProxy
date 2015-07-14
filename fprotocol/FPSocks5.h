/*
 * FPSocks5.h
 *
 *  Created on: 2012-11-6
 *      Author: AKA
 */

#ifndef FPSOCKS5_H_
#define FPSOCKS5_H_

#include "FProtocol.h"
#include <list>

namespace freeyxm {

using std::list;

//-----------------------------------------------------------------------------------------
typedef struct {
	unsigned char ver;
	union {
		unsigned char nMethods;
		unsigned char method;
	};
	union {
		unsigned char methods[255];
		unsigned char null[0];
	};
} fp_socks5_method_t;

typedef struct {
	unsigned char ver; // X'05'
	union {
		unsigned char cmd; // for request.
		unsigned char rep; // for request replay.
	};
	unsigned char rsv; // X'00'
	unsigned char atyp;
	union {
		unsigned char addr_ip4[4];
		unsigned char addr_ip6[16];
		unsigned char addr_domain[63 * 127 + 126 + 2]; // +2 for the port.
	};
	unsigned char *p_port; // dynamic modify by program base on atyp (2B). It's a bad design?
} fp_socks5_request_t;

typedef struct {
	unsigned char ver;
	union {
		unsigned char ulen;
		unsigned char status;
	};
	unsigned char username[255 + 1 + 255];
	unsigned char *p_plen; // dynamic modify by program base on ulen (1B).
	unsigned char *p_password; // dynamic modify by program base on ulen && plen.
} fp_socks5_userpass_t;

typedef struct {
	unsigned char rsv[2];
	unsigned char frag;
	unsigned char atyp;
	union {
		unsigned char addr_ip4[4];
		unsigned char addr_ip6[16];
		unsigned char addr_domain[63 * 127 + 126 + 2 + 1472];
	};
	unsigned char *p_port; // dynamic modify by program base on atyp (2B).
	unsigned char *p_data; // dynamic modify by program (1472B).
} fp_socks5_udp_t;

#define S5_MIN_METHOD_SIZE    3 // the minimum size of identifier/method selection message.
#define S5_MIN_REQUEST_SIZE   10 // the minimum size of request message.
#define S5_MAX_REQUEST_SIZE   (sizeof(fp_socks5_request_t) - sizeof(unsigned char *))
#define S5_MIN_USERPASS_SIZE  3
#define S5_MAX_USERPASS_SIZE  (sizeof(fp_socks5_userpass_t) - sizeof(unsigned char *)*2)
#define S5_MIN_UDP_SIZE       10
#define S5_MAX_UDP_SIZE       (sizeof(fp_socks5_udp_t) - sizeof(unsigned char *)*2)
//-----------------------------------------------------------------------------------------
#define S5_VERSION      0x05 // VERSION
#define S5_RESERVE      0x00 // RESERVED
// method type:
typedef unsigned char s5_method_t;
#define S5_METHOD_NONE     0x00 // NO AUTHENTICATION REQUIRED
#define S5_METHOD_GSSAPI   0x01 // GSSAPI
#define S5_METHOD_UP       0x02 // USERNAME/PASSWORD
#define S5_METHOD_IANA     0x03 // to X��7F�� IANA ASSIGNED
#define S5_METHOD_PRIVATE  0x80 // to X��FE�� RESERVED FOR PRIVATE METHODSS
#define S5_METHOD_WRONG    0xFF // NO ACCEPTABLE METHODS
// request cmd type:
#define S5_CMD_CONNECT  0x01 // CONNECT
#define S5_CMD_BIND     0x02 // BIND
#define S5_CMD_UDP      0x03 // UDP ASSOCIATE
// address type:
#define S5_ATYP_IP4     0x01 // IP V4 address
#define S5_ATYP_DOMAIN  0x03 // DOMAINNAME
#define S5_ATYP_IP6     0x04 // IP V6 address
// Reply field:
#define S5_REP_SUCCESS           0x00 // succeeded
#define S5_REP_FAILURE           0x01 // general SOCKS server failure
#define S5_REP_RULESET           0x02 // connection not allowed by ruleset
#define S5_REP_NET_UNREACHABLE   0x03 // Network unreachable
#define S5_REP_HOST_UNREACHABLE  0x04 // Host unreachable
#define S5_REP_REFUSED           0x05 // Connection refused
#define S5_REP_TTL_EXPIRED       0x06 // TTL expired
#define S5_REP_CMD_UNSUPPORTED   0x07 // Command not supported
#define S5_REP_ATYP_UNSUPPORTED  0x08 // Address type not supported
#define S5_REP_UNASSIGNED        0x09 // X'09' to X'FF' unassigned
#define S5_REP_TERMINATED        0xFF // ...
// username/password:
#define S5_USERPASS_VERSION  0x01
#define S5_USERPASS_STATUS_SUCCESS  0x00
#define S5_USERPASS_STATUS_FAILURE  0x01
// udp
#define S5_UDP_RESERVED 0x0000
#define S5_UDP_RESERVED1 0x00
#define S5_UDP_RESERVED2 0x00
//-----------------------------------------------------------------------------------------

class FP_Socks5: public FProtocol {
public:
	FP_Socks5();
	virtual ~FP_Socks5();

	int process(FSocketTcp *socket);

private:
	int dealValidate(FSocketTcp *socket);
	int dealRequest(FSocketTcp *socket);

	int choseMethod(const fp_socks5_method_t *method);
	int validate(int method, FSocketTcp *socket);
	int validateUserPass(const string &username, const string &password);

	s5_method_t parseAddrPort(fp_socks5_request_t *s5_request, const int request_size, string &dst_addr,
			unsigned int &dst_port);
	s5_method_t parseAddrPort(fp_socks5_udp_t *s5_udp, int &udp_size, string &dst_addr, unsigned int &dst_port);

	s5_method_t dealRequestConnect(FSocketTcp *socket, const fp_socks5_request_t *s5_request, const string dst_addr,
			const unsigned int dst_port);
	s5_method_t dealRequestBind(FSocketTcp *socket, const fp_socks5_request_t *s5_request, const string dst_addr,
			const unsigned int dst_port);
	s5_method_t dealRequestUdp(FSocketTcp *socket, const fp_socks5_request_t *s5_request, const string src_addr,
			const unsigned int src_port);

private:
	list<s5_method_t> method_list;
};

} /* namespace freeyxm */
#endif /* FPSOCKS5_H_ */

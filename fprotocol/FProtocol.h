/*
 * FProtocol.h
 *
 *  Created on: 2012-11-5
 *      Author: AKA
 */

#ifndef FPROTOCOL_H_
#define FPROTOCOL_H_

#include "fcore/FSocketTcp.h"
#include "fcore/FThread.h"
#include <string>

namespace freeyxm {

using std::string;

// relay buffer size:
#define RELAY_BUF_SIZE 1024*4

typedef struct {
	pthread_t tid; // another thread's id associate with new thread.
	FSocketTcp *recv_socket; // socket to recv data from.
	FSocketTcp *send_socket; // socket to send data to.
} fp_thread_param_t;

class FProtocol {
public:
	FProtocol();
	virtual ~FProtocol();

	virtual int process(FSocketTcp *socket) = 0;

	static void inet_ntop(int af, const void *cp, string &addr);

	static unsigned short ntohs(const unsigned char *p_ns);
	static void htons(const unsigned short hs, char *p_ns);
	static void ntons(const unsigned short ns, char *p_ns);

	static int onceRecvAndSend(FSocketTcp *recv_socket, FSocketTcp *send_socket, char *buf, const int size);
	static int loopRecvAndSend(FSocketTcp *socket1, FSocketTcp *socket2);
};

} /* namespace freeyxm */
#endif /* FPROTOCOL_H_ */

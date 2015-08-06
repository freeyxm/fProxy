/*
 * FSocketTcp.h
 *
 *  Created on: 2012-11-10
 *      Author: AKA
 */

#ifndef FSOCKETTCP_H_
#define FSOCKETTCP_H_

#include "FSocket.h"

namespace freeyxm {

class FSocketTcp: public FSocket {
public:
	FSocketTcp(int domain = AF_INET);
	virtual ~FSocketTcp();

	int listen(const int n = 10);
	FSocketTcp* accept();

	int recv(char *buf, const size_t size);
	int send(const char *buf, const size_t size);
	int sendAll(const char *buf, const size_t size);

	int readLine(string &line, const string line_delimiter);
};

} /* namespace freeyxm */
#endif /* FSOCKETTCP_H_ */

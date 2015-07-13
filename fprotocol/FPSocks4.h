/*
 * FPSocks4.h
 *
 *  Created on: 2012-11-5
 *      Author: AKA
 */

#ifndef FPSOCKS4_H_
#define FPSOCKS4_H_

#include "FProtocol.h"
#include <string>

namespace freeyxm {

using std::string;

typedef struct {
	char vn;
	char cd;
	char dst_port[2];
	char dst_ip[4];
	char userid[1024];
	char null;
} fp_socks4_t;

class FP_Socks4: public FProtocol {
public:
	FP_Socks4();
	virtual ~FP_Socks4();

	int checkConn(const char *);
	int checkBind(const char *);

private:
	string dst_ip;
	int dst_port;
};

} /* namespace freeyxm */
#endif /* FPSOCKS4_H_ */

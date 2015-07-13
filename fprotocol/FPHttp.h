/*
 * FPHttp.h
 *
 *  Created on: 2012-11-15
 *      Author: kill
 */

#ifndef FPHTTP_H_
#define FPHTTP_H_

#include "FProtocol.h"
#include <string>
#include <map>

namespace freeyxm {

using namespace std;

typedef map<string, string> header_map_t;

#define HEADER_LINE_DELIMITER "\r\n"

class FPHttp: public freeyxm::FProtocol {
public:
	FPHttp();
	virtual ~FPHttp();

	virtual int process(FSocketTcp *socket);

private:
	int readAndParseCmd(FSocketTcp *socket);
	int readHeaders(FSocketTcp *socket, header_map_t &headerMap);
	int parseHeaderLine(const string headerLine, string &key, string &value);
	int parseHostAndPort(const string uri, const header_map_t &headerMap,
			string &host, int &port);
private:
	string method;
	string uri;
	string version;
};

} /* namespace freeyxm */
#endif /* FPHTTP_H_ */

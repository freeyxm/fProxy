/*
 * FPHttp.cpp
 *
 *  Created on: 2012-11-15
 *      Author: kill
 */

#include "FPHttp.h"
#include "fcore/FUtil.h"
#include "fcore/FString.h"
#include <cstdlib>
#include <cstring>

namespace freeyxm {

FPHttp::FPHttp() {
	// TODO Auto-generated constructor stub

}

FPHttp::~FPHttp() {
	// TODO Auto-generated destructor stub
}

int FPHttp::process(FSocketTcp *socket) {

	/*
	 char buf[1024 + 1];
	 int nrecv = socket->recv(buf, 1024);
	 if (nrecv < 0) {
	 DEBUG_PRINTLN_ERR("recv error", socket->getErrCode(),
	 socket->getErrStr().c_str());
	 return -1;
	 }
	 buf[nrecv] = '\0';
	 DEBUG_PRINTLN_MSG(buf);
	 */

	header_map_t headers;

	if (this->readAndParseCmd(socket) > 0) {
		this->readHeaders(socket, headers);
		string host;
		int port;
		if (0 == this->parseHostAndPort(uri, headers, host, port)) {
			DEBUG_PRINT(1, ">>>Host: %s:%d\n", host.c_str(), port);
		}
	}
	socket->close();
	return 0;
}

int FPHttp::readAndParseCmd(FSocketTcp *socket) {
	string line;
	int ret = socket->readLine(line, HEADER_LINE_DELIMITER);
	if (ret <= 0) {
		return ret;
	}
	line = FString::trim(line);
	if (line.empty()) {
		return -1;
	}

	size_t pos1, pos2;
	pos1 = line.find_first_of(' ');
	if (pos1 == string::npos) { // line trimed.
		return -1;
	}
	pos2 = line.find_last_of(' ');
	if (pos2 <= pos1) { // line trimed && pos1!=npos.
		return -1;
	}

	method = FString::trim(line.substr(0, pos1));
	uri = FString::trim(line.substr(pos1 + 1, pos2 - pos1 - 1));
	version = FString::trim(line.substr(pos2 + 1));

	DEBUG_PRINT(1, ">>>CMD: method=%s,uri=%s,ver=%s.\n", method.c_str(),
			uri.c_str(), version.c_str());
	return 1;
}

int FPHttp::readHeaders(FSocketTcp *socket, header_map_t &headerMap) {
	headerMap.clear();
	string line, name, value;
	int ret;
	while (1) {
		ret = socket->readLine(line, HEADER_LINE_DELIMITER);
		if (ret <= 0) {
			break;
		}
		if (line.empty()) {
			break;
		}
		if (this->parseHeaderLine(line, name, value) < 0) {
			continue;
		}
		name = FString::tolower(name);
		headerMap.insert(header_map_t::value_type(name, value));
	}
	for (header_map_t::iterator it = headerMap.begin(); it != headerMap.end();
			++it) {
		DEBUG_PRINT(1, "%s: %s\n", it->first.c_str(), it->second.c_str());
	}
	return 0;
}

int FPHttp::parseHeaderLine(const string headerLine, string &name,
		string &value) {
	name.clear();
	value.clear();
	string line = FString::trim(headerLine);
	size_t pos = line.find_first_of(':');
	if (pos == string::npos || !pos) {
		return -1;
	}
	name = line.substr(0, pos);
	value = line.substr(pos + 1);
	return 0;
}

int FPHttp::parseHostAndPort(const string uri, const header_map_t &headerMap,
		string &host, int &port) {
	string host_port;
	size_t pos1 = uri.find("http://"), pos2;
	if (pos1 != string::npos) {
		pos1 = ::strlen("http://");
		pos2 = uri.find_first_of('/', pos1);
		if (pos2 != string::npos) {
			host_port = uri.substr(pos1, pos2 - pos1);
		} else {
			host_port = uri.substr(pos1);
		}
	} else {
		header_map_t::const_iterator it = headerMap.find("host");
		if (it == headerMap.end()) {
			return -1; // no available host found!
		}
		host_port = it->second;
	}

	host_port = FString::trim(host_port);
	if (host_port.empty()) {
		return -1;
	}
	pos1 = host_port.find_first_of(':');
	if (!pos1 || pos1 == host_port.length() - 1) { // ":x","x.x.x.x:"
		return -1;
	}
	if (pos1 == string::npos) {
		host = host_port;
		port = 80;
	} else {
		host = host_port.substr(0, pos1);
		string port2 = FString::trim(host_port.substr(pos1 + 1));
		if (!FString::isAllDigit(port2)) {
			return -1;
		}
		port = ::atoi(port2.c_str());
	}

	return 0;
}

} /* namespace freeyxm */

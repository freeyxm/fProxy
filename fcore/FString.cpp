/*
 * FString.cpp
 *
 *  Created on: 2012-11-16
 *      Author: kill
 */

#include "FString.h"
#include <cctype>

namespace freeyxm {

FString::FString() {
	// TODO Auto-generated constructor stub
}

FString::~FString() {
	// TODO Auto-generated destructor stub
}

string FString::trim(const string str) {
	if (str.empty()) {
		return string();
	}
	size_t p1 = 0, p2 = str.length() - 1;
	while (::isspace(str[p1]) && (++p1 < p2))
		;
	while (::isspace(str[p2]) && (--p2 > p1))
		;
	if (p1 <= p2) {
		return str.substr(p1, p2 - p1 + 1);
	} else {
		return string();
	}
}

string FString::tolower(const string str) {
	string r_str;
	for (string::const_iterator it = str.begin(); it != str.end(); ++it) {
		r_str += ::tolower(*it);
	}
	return r_str;
}

string FString::toupper(const string str) {
	string r_str;
	for (string::const_iterator it = str.begin(); it != str.end(); ++it) {
		r_str += ::toupper(*it);
	}
	return r_str;
}

bool FString::isAllDigit(const string str) {
	for (string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!::isdigit(*it))
			return false;
	}
	return true;
}

} /* namespace freeyxm */

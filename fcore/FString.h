/*
 * FString.h
 *
 *  Created on: 2012-11-16
 *      Author: kill
 */

#ifndef FSTRING_H_
#define FSTRING_H_

#include <string>

namespace freeyxm {

using namespace std;

class FString: public std::basic_string<char> {
public:
	FString();
	virtual ~FString();

	static string trim(const string str);
	static string tolower(const string str);
	static string toupper(const string str);
	static bool isAllDigit(const string str);
};

} /* namespace freeyxm */
#endif /* FSTRING_H_ */

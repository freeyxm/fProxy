/*
 * FUtil.h
 *
 *  Created on: 2012-10-26
 *      Author: freeyxm
 */

#ifndef FUTIL_H_
#define FUTIL_H_

#include "FLogger.h"
#include <string>

namespace freeyxm {

using std::string;

class FUtil {
public:
	FUtil();
	virtual ~FUtil();

	static void print_hex(FILE *out, const unsigned char *bytes, const int size, const int line_num);

	static int getErrCode();
	static string getErrStr();
};

} /* namespace freeyxm */
#endif /* FUTIL_H_ */

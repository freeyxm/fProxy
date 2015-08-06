/*
 * FSocketDomain6.h
 *
 *  Created on: 2015年8月6日
 *      Author: aka
 */

#ifndef FCORE_FSOCKETDOMAIN6_H_
#define FCORE_FSOCKETDOMAIN6_H_

#include <fcore/FSocketDomain.h>

namespace freeyxm {

class FSocketDomain6: public FSocketDomain {
public:
	FSocketDomain6(int socket_type);
	virtual ~FSocketDomain6();

	virtual void setLocalAddress(const struct sockaddr* addr);
	virtual void setRemoteAddress(const struct sockaddr* addr);

	virtual socklen_t getAddrLen();
};

} /* namespace freeyxm */

#endif /* FCORE_FSOCKETDOMAIN6_H_ */

/*
 * FSocketDomain4.h
 *
 *  Created on: 2015年8月6日
 *      Author: aka
 */

#ifndef FCORE_FSOCKETDOMAIN4_H_
#define FCORE_FSOCKETDOMAIN4_H_

#include <fcore/FSocketDomain.h>

namespace freeyxm {

class FSocketDomain4: public FSocketDomain {
public:
	FSocketDomain4(int socket_type);
	virtual ~FSocketDomain4();

	virtual void setLocalAddress(const struct sockaddr* addr);
	virtual void setRemoteAddress(const struct sockaddr* addr);

	virtual socklen_t getAddrLen();
};

} /* namespace freeyxm */

#endif /* FCORE_FSOCKETDOMAIN4_H_ */

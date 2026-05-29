/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2LGWCOMMCONTROLLER_H_
#define _CCU2LGWCOMMCONTROLLER_H_

#include <CCU2CommController.h>
#include <string>

namespace HM2 {

class CCU2LGWCommController : public CCU2CommController
{
public:
	CCU2LGWCommController(CCU2BidcosRemoteInterface* bidcosRemoteInterface);
	virtual ~CCU2LGWCommController();

	bool init(const std::string host, const int port, const std::string& encKey, const std::string& desiredSerial, const bool csmacaEnabled, const bool imporovedCoproInit);

	bool setRFLGWInfoLED(const unsigned int state);

	/* \brief Reinitialized coprocessor after gateway reconnect.
	 * \details Calls initCoprocessor, waits for and returns result
	 */
	bool reinitCoprocessor();

private:
	std::string calculateMD5(const std::string& s);

};

} //namespace

#endif

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2LGWCOMMCONTROLLERMOD_H_
#define _CCU2LGWCOMMCONTROLLERMOD_H_

#include <CCU2CommControllerMod.h>
#include <string>

namespace HM2Mod {

class CCU2LGWCommControllerMod : public CCU2CommControllerMod
{
public:
	CCU2LGWCommControllerMod();
	virtual ~CCU2LGWCommControllerMod();

	bool init(const std::string host, const int port, const std::string& encKey, const std::string& desiredSerial, bool skipStartApp = false);

	bool setRFLGWInfoLED(const unsigned int state);


private:
	std::string calculateMD5(const std::string& s);

};

} //namespace

#endif

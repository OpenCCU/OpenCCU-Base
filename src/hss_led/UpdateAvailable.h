/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#ifndef UPDATEAVAILABLE_H_
#define UPDATEAVAILABLE_H_
#if !defined(PLATFORM_CCU3)
#include "Info.h"
#include <cstdint>
#include <string>
class UpdateAvailable:public Info {
public:
	UpdateAvailable();
	virtual ~UpdateAvailable();
	virtual bool isInfoPending();
private:
	bool updateAvailable;
	std::string currentVersion;
	std::string serialNumber;
	std::string availableVersion;
	uint64_t lastUpdateServerRequest;
	uint64_t waitRequesTime;
	bool requestUpdateServer();

};
#endif

#endif /* UPDATEAVAILABLE_H_ */

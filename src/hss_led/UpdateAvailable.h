/*
 * UpdateAvailable.h
 *
 *  Created on: 18.01.2013
 */

#ifndef UPDATEAVAILABLE_H_
#define UPDATEAVAILABLE_H_
#if !defined(PLATFORM_CCU3)
#include "Info.h"
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

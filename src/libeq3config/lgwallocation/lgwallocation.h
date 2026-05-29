/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LGWALLOCATION_H_
#define _LGWALLOCATION_H_

#include <string>
#include <vector>

class LGWAllocation {

public:
	LGWAllocation();
	
	std::string deviceTypeToFWMapType(const std::string& deviceType);
	std::string deviceTypeToBidcosType(const std::string& deviceType);
	
	std::string fwmapTypeToBidcosType(const std::string& fwmapType);	

	std::string bidcosTypeToFWMapType(const std::string& bidcosType);	

	bool deviceTypeFilterMatches(const std::string& deviceTypeFilter, const std::string& deviceType);
	
	std::vector<std::string> getFWMapTypes() const;
	std::vector<std::string> getBidcosTypes() const;
private:
	struct LGWType {
		std::string deviceType;//!< Devicetype returned by the gateway.
		std::string fwmapType;//!< Typestring in fwmap file
		std::string bidcosType;//!< Typestring in rfd.conf/hs485d.conf, webui and bidcos services	
	};
	
	std::vector<LGWType> lgwTypeList;
};

#endif


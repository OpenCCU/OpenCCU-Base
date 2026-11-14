/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "lgwallocation.h"


LGWAllocation::LGWAllocation()
{
	LGWType t;
	
	//(Wired LAN Gateway)
	t.deviceType = "eQ3-HMW-LGW*";
	t.fwmapType = "HMW-LGW-O-DR-GS-EU";
	t.bidcosType = "HMWLGW";
	lgwTypeList.push_back(t);
	//(RF-LGW 2nd  generation)
	t.deviceType = "eQ3-HM-LGW*";
	t.fwmapType = "HM-LGW-O-TW-W-EU";//TODO Not released
	t.bidcosType = "HMLGW2";
	lgwTypeList.push_back(t);	
}


//---------------------------------------------------------------------------------

std::string LGWAllocation::deviceTypeToFWMapType(const std::string& deviceType)
{
	std::string val;
	if(!deviceType.empty()) {
		for(unsigned int i = 0; i < lgwTypeList.size(); i++) {
			LGWType& t = lgwTypeList.at(i);
			if(deviceTypeFilterMatches(t.deviceType, deviceType)) {
				val = t.fwmapType;
			}
		}
	}
	return val;
}

std::string LGWAllocation::deviceTypeToBidcosType(const std::string& deviceType)
{
	std::string val;
	if(!deviceType.empty()) {
		for(unsigned int i = 0; i < lgwTypeList.size(); i++) {
			LGWType& t = lgwTypeList.at(i);
			if(deviceTypeFilterMatches(t.deviceType, deviceType)) {
				val = t.bidcosType;
			}
		}
	}
	return val;
}
	
//---------------------------------------------------------------------------------

std::string LGWAllocation::fwmapTypeToBidcosType(const std::string& fwmapType)
{
	std::string val;
	if(!fwmapType.empty()) {
		for(unsigned int i = 0; i < lgwTypeList.size(); i++) {
			LGWType& t = lgwTypeList.at(i);
			if(t.fwmapType.compare(fwmapType) == 0) {
				val = t.bidcosType;
			}
		}
	}
	return val;
}

//---------------------------------------------------------------------------------

std::string LGWAllocation::bidcosTypeToFWMapType(const std::string& bidcosType)
{
	std::string val;
	if(!bidcosType.empty()) {
		for(unsigned int i = 0; i < lgwTypeList.size(); i++) {
			LGWType& t = lgwTypeList.at(i);
			if(t.bidcosType.compare(bidcosType) == 0) {
				val = t.fwmapType;
			}
		}
	}
	return val;	
}

//---------------------------------------------------------------------------------

bool LGWAllocation::deviceTypeFilterMatches(const std::string& deviceTypeFilter, const std::string& deviceType)
{
	bool matches = false;
	if( (deviceTypeFilter.size() == 0) && deviceType.size() == 0) {
		matches = true;
	}
	for(unsigned int i = 0; i < deviceTypeFilter.size(); i++) {
		if(deviceTypeFilter.at(i) == '*') {
			matches = true;
			break;
		}
		else if(i < deviceType.size()) {
			if(deviceType.at(i) != deviceTypeFilter.at(i)) {
				//matches = false;
				break;
			}
		}
		else if(((i+1) < deviceType.size()) && (deviceTypeFilter.at(i+1) == '*') ) {//reached end of deviceType without mismatch
			matches = true;
			break;//next filter char is *, good
		}
		else {
			break;//next filter char is not *, bad
		}
	}
	return matches;
}

//---------------------------------------------------------------------------------

std::vector<std::string> LGWAllocation::getFWMapTypes() const 
{
	std::vector<std::string> fwtypes;
	for(unsigned int i = 0; i < lgwTypeList.size(); i++) {
		fwtypes.push_back(lgwTypeList.at(i).fwmapType);
	}
	return fwtypes;
}

//---------------------------------------------------------------------------------

std::vector<std::string> LGWAllocation::getBidcosTypes() const
{
	std::vector<std::string> bidcosTypes;
	for(unsigned int i = 0; i < lgwTypeList.size(); i++) {
		bidcosTypes.push_back(lgwTypeList.at(i).bidcosType);
	}
	return bidcosTypes;
}


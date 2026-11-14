/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#include "Info.h"
#include "hss_led_util.h"
#include <fstream>

const std::string Info::emtpyStr = "";
const std::string Info::ConfigValue::IGNORE_SERVICE_MESSAGES = "IgnoreServiceMessages";
const std::string Info::ConfigValue::IGNORE_ALARM_MESSAGES = "IgnoreAlarmMessages";

Info::Info() {
	// TODO Automatisch generierter Konstruktorstub
}

Info::~Info() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

std::map<std::string, std::string> Info::readConfig() {
	std::map<std::string, std::string> cfgValues;
	std::ifstream is;
	is.open("/etc/config/hss_led_info.conf", std::ifstream::in);
	if(is.is_open())  {
		std::string line;
		while ( getline(is, line) ) {
			line = HSSLedUtil::trim(line);
			if(!line.empty()) {
				std::string::size_type index = line.find_first_of("=",0);
				if( (index != std::string::npos) && (index+1 < line.size())) {
					std::string key;
					std::string value;
					key = line.substr(0, index);
					value = line.substr(index+1);	
					key = HSSLedUtil::trim(key);
					value = HSSLedUtil::trim(value);
					if((!key.empty()) && (!value.empty())) {
						cfgValues[key] = value;
					}
				}
				
			}
		}
		is.close();
	}
	return cfgValues;
}

bool Info::isInfoEnabled(const std::map<std::string, std::string>& configData) {
	return true; //default implmentation. May be reimplemented by iheriting classes
}

const std::string& Info::getConfigValue(const std::map<std::string, std::string>& configData, const std::string& key) {
	std::map<std::string,std::string>::const_iterator cit = configData.find(key);
	if(cit != configData.end()) {
		return cit->second;
	}
	else {
		return emtpyStr;
	}
}

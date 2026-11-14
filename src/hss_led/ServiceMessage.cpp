/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * ServiceMessage.cpp
 *
 *  Created on: 18.01.2013
 */

#include "ServiceMessage.h"
#include "hss_led_util.h"

ServiceMessage::ServiceMessage() 
{
}

ServiceMessage::~ServiceMessage() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

bool ServiceMessage::isInfoEnabled(const std::map<std::string, std::string>& configData) {
	return !(HSSLedUtil::stringToBool(getConfigValue(configData, ConfigValue::IGNORE_SERVICE_MESSAGES) ));
}

bool ServiceMessage::isInfoPending() {
	std::map<std::string,int>::iterator it;
	for(it = messages.begin();it != messages.end();++it)
	{
		if(it->second > 0)
		{
			return true;
		}
	}
	return false;
}

void ServiceMessage::setMessage(std::string souce, int value) {
	messages[souce] = value;
}
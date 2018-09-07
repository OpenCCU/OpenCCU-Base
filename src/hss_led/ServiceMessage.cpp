/*
 * ServiceMessage.cpp
 *
 *  Created on: 18.01.2013
 *      Author: willms
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
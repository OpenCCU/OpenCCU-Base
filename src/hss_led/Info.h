/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef INFO_H_
#define INFO_H_

#include <map>
#include <string>

class Info {
public:
	Info();
	virtual ~Info();
	virtual bool isInfoPending() = 0;
	virtual bool isInfoEnabled(const std::map<std::string, std::string>& configData);
	
	static std::map<std::string, std::string> readConfig();
	static const std::string& getConfigValue(const std::map<std::string, std::string>& configData, const std::string& key);

	struct ConfigValue {
		static const std::string IGNORE_SERVICE_MESSAGES;
		static const std::string IGNORE_ALARM_MESSAGES;
	};


private:
	static const std::string emtpyStr;
};

#endif /* INFO_H_ */

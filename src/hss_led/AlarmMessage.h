/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef ALARMMESSAGE_H_
#define ALARMMESSAGE_H_
#include <map>
#include "Info.h"
#include <string>
class AlarmMessage: public Info {
public:
	AlarmMessage();
	virtual ~AlarmMessage();
	virtual bool isInfoPending();
	virtual bool isInfoEnabled(const std::map<std::string, std::string>& configData);
	void setMessage(std::string source, bool value);
private:
	std::map<std::string,bool> messages;
};

#endif /* ALARMMESSAGE_H_ */

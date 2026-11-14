/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * ServiceMessage.h
 *
 *  Created on: 18.01.2013
 */

#ifndef SERVICEMESSAGE_H_
#define SERVICEMESSAGE_H_
#include <map>
#include <string>
#include "Info.h"

class ServiceMessage: public Info {
public:
	ServiceMessage();
	virtual ~ServiceMessage();
	virtual bool isInfoPending();
	virtual bool isInfoEnabled(const std::map<std::string, std::string>& configData);
	void setMessage(std::string souce, int value);
private:
	std::map<std::string,int> messages;

};

#endif /* SERVICEMESSAGE_H_ */

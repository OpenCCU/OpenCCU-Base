/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CRYPTTOOLCONFIG_H_
#define _CRYPTTOOLCONFIG_H_

#include <string>

class crypttoolconfig
{
private:
	std::string configfilePath;

public:
	/**\brief Default constructor using default config file path /etc/config/crypttool.cfg */
	crypttoolconfig();
	/**\brief Constructor using given path to config file. */
	crypttoolconfig(const std::string& configfilePath);

	virtual ~crypttoolconfig(void);

	std::string getCurrentKey(void);

	int getCurrentKeyIndex(void);

	int setCurrentKey(int index, std::string key);

	int resetConfig(void);
};

#endif

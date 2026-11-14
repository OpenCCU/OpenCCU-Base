/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANUTILS_NETWORKINTERFACEHELPER_H_
#define _LIBLANUTILS_NETWORKINTERFACEHELPER_H_

#include <vector>
#include <string>

class NetworkInterfaceHelper
{
public:
	NetworkInterfaceHelper(void);
	~NetworkInterfaceHelper(void);

public:
	std::vector<std::string> getIPv4Adresses();
};

#endif
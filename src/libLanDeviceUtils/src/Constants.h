/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_CONSTANTS_H_
#define _LIBLANDEVICEUTILS_CONSTANTS_H_

namespace LDU {

class Constants {
public:
	//! Constant containing IPv4 multicast address 224.0.0.1
	static const char* MULTICAST_ADDRESS;
	//! Constant containing IPv4 broadcast address 255.255.255.255
	static const char* BROADCAST_ADDRESS;
};
}
#endif
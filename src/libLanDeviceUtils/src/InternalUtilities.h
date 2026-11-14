/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_INTERNAL_UTILITIES_H_
#define _LIBLANDEVICEUTILS_INTERNAL_UTILITIES_H_

#include <string>
#include <stdio.h>

namespace LDU {

/** \brief Extracts ip address components from string.
 * \details Extracted components will be written to given int parameter.
 * \param address Address to extract components from.
 * \param first First component (192.168.1.2 <-> 192)
 * \param second Second component (192.168.1.2 <-> 168)
 * \param third Third component (192.168.1.2 <-> 1)
 * \param fourth Fourth component (192.168.1.2 <-> 2)
 */
inline void extractIPAddressComponents(const std::string& address, int& first, int& second, int& third, int& fourth) {
#ifdef WIN32 //sscanf_s is a microsoft specific function which would break gcc compilability
	#pragma warning(disable : 4996)
#endif
	sscanf(address.c_str(), "%d.%d.%d.%d", &first, &second, &third, &fourth);
}

/** \brief Assembles an ip string from components.
 * \param first First component (192.168.1.2 <-> 192)
 * \param second Second component (192.168.1.2 <-> 168)
 * \param third Third component (192.168.1.2 <-> 1)
 * \param fourth Fourth component (192.168.1.2 <-> 2)
 * \param address Result string.
 */
inline void assembleIPAddressFromComponents(const int first, const int second, const int third, const int fourth, std::string& address) {
	char* buffer = new char[20];
	address.clear();
	snprintf(buffer, 20, "%u.%u.%u.%u", (unsigned int)first, (unsigned int)second, (unsigned int)third, (unsigned int)fourth);
	address.append(buffer);
	delete[] buffer;
}

inline std::string assembleIpAddressStringFromByteNumbers(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
#ifdef WIN32 //sscanf_s is a microsoft specific function which would break gcc compilability
	#pragma warning(disable : 4996)
#endif
	std::string result;
	char* buffer = new char[20];
	snprintf(buffer, 20, "%u.%u.%u.%u", (unsigned int)a, (unsigned int)b, (unsigned int)c, (unsigned int)d);
	result.append(buffer);
	delete[] buffer;
	return result;
}
}

#endif

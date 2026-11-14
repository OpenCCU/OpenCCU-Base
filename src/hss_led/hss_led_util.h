/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/
#ifndef _HSS_LED_UTIL_H_
#define _HSS_LED_UTIL_H_ 

#include <string>

class HSSLedUtil {
public:

    static std::string trim(const std::string& str);
    static bool stringToBool(const std::string& str, const bool defaultValue = false);
};

#endif
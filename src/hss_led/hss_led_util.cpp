/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/
#include "hss_led_util.h"
#include <sstream>

std::string HSSLedUtil::trim(const std::string& str) {
    std::string::size_type first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    std::string::size_type last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool HSSLedUtil::stringToBool(const std::string& str, const bool defaultValue /*=false*/) {
    bool theBool = defaultValue;
    try {
        std::istringstream(str) >> std::boolalpha >> theBool;
    } 
    catch(...) {
    }
    return theBool;
}

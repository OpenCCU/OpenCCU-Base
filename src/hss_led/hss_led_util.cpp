/*
* Copyright (C) 2018 eQ-3 Entwicklung GmbH
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

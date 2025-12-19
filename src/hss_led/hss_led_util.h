/*
* Copyright (C) 2018 eQ-3 Entwicklung GmbH
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
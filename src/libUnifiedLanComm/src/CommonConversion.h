/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _COMMONCONVERSION_H_
#define _COMMONCONVERSION_H_


#include <DLLImportExportULC.h>
#include <string>

LIBUNIFIEDLANCOMM_API std::string toString(unsigned char c);
LIBUNIFIEDLANCOMM_API std::string toHexString(const std::string& str);
LIBUNIFIEDLANCOMM_API std::string hexStringToString(const std::string& hexStr);


#endif

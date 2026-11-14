/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _UNIFIEDLANPROTOCOLMESSAGETYPECONVERTER_H_
#define _UNIFIEDLANPROTOCOLMESSAGETYPECONVERTER_H_

#include <string>
#include <DLLImportExportULC.h>

LIBUNIFIEDLANCOMM_API int hexStringToInt(const std::string& hexStr);
LIBUNIFIEDLANCOMM_API unsigned int hexStringToUInt(const std::string& hexStr);
LIBUNIFIEDLANCOMM_API unsigned char hexStringToUChar(const std::string& hexStr);

LIBUNIFIEDLANCOMM_API std::string binaryDataToHexStr(const std::string& binaryData);
LIBUNIFIEDLANCOMM_API std::string intToHexStr(const int i);
LIBUNIFIEDLANCOMM_API std::string ucharToHexStr(const unsigned char uc);

#endif

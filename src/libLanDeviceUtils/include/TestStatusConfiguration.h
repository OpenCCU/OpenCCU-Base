/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_TESTSTATUSCONFIGURATION_H_
#define _LIBLANDEVICEUTILS_TESTSTATUSCONFIGURATION_H_

#include <string>

namespace LDU {

#include <DLLImportExport.h>

class TestStatusConfiguration
{
public:
	LIBLANDEVICEUTILS_API TestStatusConfiguration(void);
	LIBLANDEVICEUTILS_API virtual ~TestStatusConfiguration(void);

//VARIABLES
private:
	unsigned char testStatus;

//METHODS

public:
	LIBLANDEVICEUTILS_API const unsigned char getTestStatus() const;
	LIBLANDEVICEUTILS_API void setTestStatus(const unsigned char testStatus);

	LIBLANDEVICEUTILS_API std::string toString() const;
public:
};
}
#endif

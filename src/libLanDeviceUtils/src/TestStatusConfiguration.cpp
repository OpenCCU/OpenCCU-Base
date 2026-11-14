/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "TestStatusConfiguration.h"
#include <sstream>
#include <stdio.h>

using namespace LDU;

#ifdef WIN32
# define snprintf _snprintf
#endif

TestStatusConfiguration::TestStatusConfiguration(void)
{
}

TestStatusConfiguration::~TestStatusConfiguration(void)
{
}

const unsigned char TestStatusConfiguration::getTestStatus() const
{
	return this->testStatus;
}

void TestStatusConfiguration::setTestStatus(const unsigned char testStatus)
{
	this->testStatus = testStatus;
}

std::string TestStatusConfiguration::toString() const
{
	char buffer[10];
	snprintf(buffer, 10, "%X", this->testStatus);

	std::string s;
	s.append("----------------\n");
	s.append("Teststatus: "); s.append(buffer); s.append("\n");
	s.append("----------------\n");
	return s;
}

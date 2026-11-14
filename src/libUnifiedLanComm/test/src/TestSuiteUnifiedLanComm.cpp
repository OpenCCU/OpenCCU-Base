/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


//Googles gtest
#include "gtest/gtest.h"

//Test for CommonConversion.h
#include "TestCommonConversion.h"
#include "TestUnifiedLanProtocolTypeConverter.h"


int main(int argc, char**argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


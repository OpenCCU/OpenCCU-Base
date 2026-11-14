/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

//gtest stuff
#include "gtest/gtest.h"

#include <UnifiedLanProtocolTypeConverter.h>

namespace {
// The fixture for testing class Foo.
class TestUnifiedLanProtocolConverter : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

	TestUnifiedLanProtocolConverter() {
    // You can do set-up work for each test here.
  }

  virtual ~TestUnifiedLanProtocolConverter() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};


TEST_F(TestUnifiedLanProtocolConverter, hexStringToInt) {
		ASSERT_EQ(1, hexStringToInt(std::string("1")));
		ASSERT_EQ(1, hexStringToInt(std::string("01")));
		ASSERT_EQ(16, hexStringToInt(std::string("10")));
		ASSERT_EQ(10, hexStringToInt(std::string("A")));
		ASSERT_EQ(161, hexStringToInt(std::string("A1")));
		ASSERT_EQ(26, hexStringToInt(std::string("1A")));
		ASSERT_EQ(0, hexStringToInt(std::string("00")));
		ASSERT_EQ(0, hexStringToInt(std::string("0 0")));
		ASSERT_EQ(255, hexStringToInt(std::string("FF")));
		ASSERT_EQ((int)-1, hexStringToInt(std::string("FFFFFFFF")));
		ASSERT_EQ(2147483647, hexStringToInt(std::string("7FFFFFFF")));
		int i = -2147483648;
		ASSERT_EQ(i, hexStringToInt(std::string("80000000")));
}

TEST_F(TestUnifiedLanProtocolConverter, hexStringToUInt) {
		ASSERT_EQ(1, hexStringToUInt(std::string("1")));
		ASSERT_EQ(1, hexStringToUInt(std::string("01")));
		ASSERT_EQ(16, hexStringToUInt(std::string("10")));
		ASSERT_EQ(10, hexStringToUInt(std::string("A")));
		ASSERT_EQ(161, hexStringToUInt(std::string("A1")));
		ASSERT_EQ(26, hexStringToUInt(std::string("1A")));
		ASSERT_EQ(0, hexStringToUInt(std::string("00")));
		ASSERT_EQ(0, hexStringToUInt(std::string("0 0")));
		ASSERT_EQ(255, hexStringToUInt(std::string("FF")));
		ASSERT_EQ((unsigned int)4294967295, hexStringToUInt(std::string("FFFFFFFF")));
		ASSERT_EQ(2147483647, hexStringToUInt(std::string("7FFFFFFF")));
		unsigned int i = 2147483648;
		ASSERT_EQ(i, hexStringToUInt(std::string("80000000")));
}

TEST_F(TestUnifiedLanProtocolConverter, hexStringToUChar) {
		ASSERT_EQ(1, hexStringToUChar(std::string("1")));
		ASSERT_EQ(1, hexStringToUChar(std::string("01")));
		ASSERT_EQ(16, hexStringToUChar(std::string("10")));
		ASSERT_EQ(10, hexStringToUChar(std::string("A")));
		ASSERT_EQ(161, hexStringToUChar(std::string("A1")));
		ASSERT_EQ(26, hexStringToUChar(std::string("1A")));
		ASSERT_EQ(0, hexStringToUChar(std::string("00")));
		ASSERT_EQ(0, hexStringToUChar(std::string("0 0")));
		ASSERT_EQ(255, hexStringToUChar(std::string("FF")));
		ASSERT_EQ(255, hexStringToUChar(std::string("FFFF")));
}

TEST_F(TestUnifiedLanProtocolConverter, intToHexStr)
{
	ASSERT_STRCASEEQ("1", intToHexStr(1).c_str());
	ASSERT_STRCASEEQ("0", intToHexStr(0).c_str());
	ASSERT_STRCASEEQ("FF", intToHexStr(255).c_str());
	ASSERT_STRCASEEQ("100", intToHexStr(256).c_str());
	ASSERT_STRCASEEQ("FFFFFFFF", intToHexStr(-1).c_str());//machine dependent
}

}//namespace





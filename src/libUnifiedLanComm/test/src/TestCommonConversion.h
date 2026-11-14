/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

//gtest stuff
#include "gtest/gtest.h"

#include <CommonConversion.h>

namespace {
// The fixture for testing class Foo.
class CommonConversionTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

	CommonConversionTest() {
    // You can do set-up work for each test here.
  }

  virtual ~CommonConversionTest() {
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


TEST_F(CommonConversionTest, toString_char) {
		unsigned char c = 0;
		std::string s = toString(c);
		ASSERT_STREQ("0", s.c_str());

		c = 0xff;
		s = toString(c);
		ASSERT_STREQ("255", s.c_str());

		c = 42;
		s = toString(c);
		ASSERT_STREQ("42", s.c_str());
}

TEST_F(CommonConversionTest, toHexString_hexStringToString) {
	const std::string s("The quick brown fox jumps into a wall of stone.");
	std::string hex = toHexString(s);
	std::string result = hexStringToString(hex);
	EXPECT_EQ(s, result);
}




}//namespace



/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include "MD5Module.h"
#include "md5.h"
#include <string.h>

MD5Module::MD5Module() : Command("md5")
{
}

MD5Module::~MD5Module() {
}

int MD5Module::execute()
{
	if(params.size() != 2) {
		printf("%s", help().c_str());
		return 1;
	}
	if((params.at(0).compare("-s") == 0) && (params.at(1).size() > 0)) {
		std::string result = calculateMD5(params.at(1));
		for(unsigned int i = 0; i < result.size(); i++) {
			printf("%02X", ((int)(result.at(i) & 0xFF)));
		}
		return 0;
	}
	else {
		return 2;
	}
}

std::string MD5Module::help()
{
	std::string h("md5\n\n");
	h.append("Calculates md5 checksum for a string and prints it out as hexadecimal string\n");
	h.append("Usage:\nmd5 <-s String>\n");
	h.append("\t-s String : Calculate MD5 checksum for given string.\n\n");
//	h.append("\t-f File   : Calculate MD5 checksum for given file.\n"); 
//	h.append("\t-o File   : Write output into file instead of console.\n");
	return h;
}

std::string MD5Module::calculateMD5(const std::string& s)
{
	md5 md5_calculator;
	unsigned char* buffer = new unsigned char[s.length()];
	memcpy(buffer, s.c_str(), s.length());
	md5_calculator.Update(buffer, s.length());
	md5_calculator.Finalize();
	delete buffer;

	std::string digest;

	digest.append((const char*) md5_calculator.Digest(),
	std::string::size_type(16));
	return digest;
}

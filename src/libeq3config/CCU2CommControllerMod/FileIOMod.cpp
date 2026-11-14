/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <FileIOMod.h>

#include <stdio.h>
#include <cstring>

using namespace HM2Mod;

bool FileIOMod::readStringFromFile(const std::string& filepath, std::string& result)
{
	result.clear();
	bool done = false;
	const int bufferSize = 1024;
	FILE* pFile;
	char* pBuffer = new char[bufferSize];
	pFile = fopen( filepath.c_str(), "r");
	if(pFile != NULL) {
		char* c = fgets( pBuffer, bufferSize, pFile);
		while (c != NULL) {
			result.append( pBuffer );
			memset(pBuffer, 0, sizeof(char)*bufferSize);
			c = fgets( pBuffer, bufferSize, pFile);
		}
		fclose(pFile);
		done = true;
	}
	delete[] pBuffer;
	return done;
}

bool FileIOMod::writeStringToFile(const std::string& filepath, const std::string& str)
{
	if(filepath.empty()) {
		return false;
	}
	FILE* pFile = fopen( filepath.c_str(), "w");
	if(pFile != NULL) {
		fputs(str.c_str(), pFile); 
		fflush(pFile);
		fclose(pFile);
		return true;
	}
	return false;
}

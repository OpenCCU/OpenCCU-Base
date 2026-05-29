/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * commandsgenerator.cpp
 *
 *  Created on: Mar 7, 2013
 *      Author: user
 */

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

struct CommandDefInfo {
	std::string header;
	std::string className;
};

void printUsage() {
	printf("\ncommandsgenerator <PATH/module.def> [PATH/module.def] [PATH/module.def]...\n\n");
}

static bool IsWhitespace(char c)
{
	return ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
}

void Trim(std::string& value)
{
	const char* cstr = value.c_str();

	// trim leading whitespace
	while (IsWhitespace(cstr[0]) == true)
	{
		cstr++;
	}

	// compute size
	size_t size = 0;
	int i;
	for (size_t i = 0; cstr[i] != '\0'; i++)
	{
		if (IsWhitespace(cstr[i]) == false)
		{
			size = i+1;
		}
	}

	value = std::string(cstr, size);
}

std::string getLine(const std::string& str, const int offset, int& nextLineOffset)
{
	std::string line;
	nextLineOffset = -1;
	std::string::size_type index = str.find("\n", offset);
	if(index != std::string::npos) {
		if(index+1 < str.size()) {
			nextLineOffset = index+1;
		}
		line = str.substr(offset, index-offset);
	}
	else {
		line.assign(str.substr(offset));
	}
	return line;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		printUsage();
		return 1;
	}
	for(unsigned int k = 1; k < argc ; k++) {
		std::string filepath(argv[k]);

		std::string str;
		str.reserve(1024);
		char* buffer = new char[1024];
		FILE* file = fopen(argv[k], "r");
		if(!file) {
			return 2;
		}
		memset(buffer, 0 ,1024);
		int read = fread(buffer, 1024, 1, file);
		if(read == -1) {
			fclose(file);
			return 3;
		}
		str.assign(buffer);
		delete[] buffer;
		int offset = 0;
		CommandDefInfo info;
		while(offset >= 0) {
			std::string line = getLine(str, offset, offset);
			Trim(line);
			std::string::size_type idxColon = line.find(":");
			if(idxColon == std::string::npos) {
				continue;
			}
			std::string key = line.substr(0, idxColon);
			Trim(key);
			std::string value = line.substr(idxColon+1, line.size()-idxColon-1);
			Trim(value);
			if(key.compare("HEADER") == 0) {
				info.header = value;
			}
			else if(key.compare("CLASSNAME") == 0) {
				info.className = value;
			}
		}
		if((!info.className.empty()) && (!info.header.empty())) {
			FILE* file = fopen("Commands.cpp", "a");
			if(file) {
				std::string outFileText;
				outFileText.append("#include <");
				outFileText.append(info.header);
				outFileText.append(">\n");
				outFileText.append("static ");
				outFileText.append(info.className);
				outFileText.append(" ");
				outFileText.append(info.className);
				outFileText.append("Instance");
				outFileText.append(";\n\n");
				fwrite(outFileText.c_str(), outFileText.size(), 1, file);
				fflush(file);
				fclose(file);
		//		printf("\nOutfileText:\n%s\n", outFileText.c_str());
			}
		}
	}

	return 0;
}

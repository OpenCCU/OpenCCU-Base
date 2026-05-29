/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "wait-for-file.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <utils.h>

#ifndef WIN32
 #include <unistd.h>
#endif


WaitForFile::WaitForFile() : Command("wait-for-file")
{
}

int WaitForFile::execute()
{
	std::string filepath;
	int timeout = -1;
	int pollingInterval = -1;

	const int paramsSize = static_cast<int>(params.size());
	for(int i = 0; i < paramsSize ; i++)
	{
		if((params.at(i).compare("-f") == 0) && ((i+1)<paramsSize) ) {
			i++;
			filepath = params.at(i);
		}
		else if((params.at(i).compare("-p") == 0) && ((i+1)<paramsSize) ) {
			i++;
			pollingInterval = atoi(params.at(i).c_str());
		}
		else if((params.at(i).compare("-t") == 0) && ((i+1)<paramsSize) ) {
			i++;
			timeout = atoi(params.at(i).c_str());
		}
	}
	//check for mandatory params
	if(filepath.empty() || timeout < 0 || pollingInterval < 0) {
		printUsage();
		return 1;
	}
	const uint64_t startTime = time_millis();
	while( (startTime + ((uint64_t)timeout*1000)) > time_millis()) {
		std::ifstream iStream(filepath.c_str());
		if( iStream.good() ) {
			iStream.close();
			return 0;
		}
		sleep(pollingInterval);
	}
	return 2;
}

std::string WaitForFile::help()
{
	std::string usage("wait-for-file\n");
	usage.append("Usage:\n");
	usage.append("wait-for-file <-f FILE_TO_WAIT_FOR> <-p POLLING_INTERVAL> <-t TIMEOUT>\n\n");
	usage.append("\t-f: Path of file to wait for. (Waits until file exists).\n");
	usage.append("\t-p: Polling interval in seconds.\n");
	usage.append("\t-t: Timeout in seconds.\n");
	usage.append("\n");
	usage.append("Example:\nwait-for-file -f /var/status/myfile -p 2 -t 30\n");
	return usage;
}

void WaitForFile::printUsage()
{
	std::cout << help();
}

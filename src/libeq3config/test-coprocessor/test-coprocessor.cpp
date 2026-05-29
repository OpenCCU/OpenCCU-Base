/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "test-coprocessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

#include <LanDeviceUtils.h>


#include <CCU2CoprocessorCommandMod.h>
#include <CCU2LGWCommControllerMod.h>
#include <CCU2SerialPortCommControllerMod.h>
#include <CCU2CommControllerMod.h>
//#include <Logger.h>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <md5.h>
#include <PropertyMap.h>
#include <utils.h>
#include <float.h>
#include <algorithm>
#include <math.h>

#ifndef WIN32
 #include <unistd.h>
#endif

using namespace std;
using namespace HM2Mod;

CoprocessorTest::CoprocessorTest() : Command("test-coprocessor")
, accessFile("/dev/null") //default
//, resetFile("/dev/ccu2-ic200") //default
, resetFile("/dev/null") //default
{
}

std::string CoprocessorTest::help()
{
	std::string usage("test-coprocessor\n");
	usage.append("Usage:\n");
	usage.append("test-coprocessor <-p port> [-syslog] [-l LOGLEVEL] [-v] [-i ITERATIONS]\n");
	usage.append("\t-p: Serial port of CCU2/HM-MOD-UART coprocessor.\n");
	usage.append("\t-syslog: Use syslog instead of console logging.\n");
	usage.append("\t-l: Set log level from 0 (ALL) to 5 (ERROR)\n");
	usage.append("\t-v: Verbose output. Prints out every measurement iteration time.\n");
	usage.append("\t-i: Number of measurement iterations (requests from coprocessor). Default: 1000.\n");
	return usage;
}

int CoprocessorTest::execute()
{
	HM2Mod::CCU2CommControllerMod* pPortWrapper = NULL;
	std::string port;
	bool logToConsole = true;
	int logLevel = 2;
	bool paramError = false;
	bool verboseOutput = false;
	int iterations = 1000;

	const int paramsSize = static_cast<int>(params.size());
	for(int i = 0; i < paramsSize ; i++) 
	{
		if((params.at(i).compare("-p") == 0) && ((i+1)<paramsSize) ) {
			i++;
			port.assign(params.at(i));
		}
		else if((params.at(i).compare("-syslog") == 0)) {
			logToConsole = false;
		}
		else if((params.at(i).compare("-l") == 0) && ((i+1)<paramsSize) ) {
			i++;
			logLevel = atoi(params.at(i).c_str());
		}
		else if(params.at(i).compare("-v") == 0) {
			verboseOutput = true;
		}
		else if(params.at(i).compare("-i") == 0 && ((i+1)<paramsSize)) {
			i++;
			iterations = atoi(params.at(i).c_str());
		}
		else {
			paramError = true;
		}
	}


	if(logToConsole) {
		logger = new ConsoleLogger();		
	}
	else {
		logger = new SyslogLogger("test-coprocessor");
	}
	logger->SetLevel((Logger::LogLevel)logLevel);
	
	//Parse args
	if(paramError || params.size() < 1) {
		printUsage();
		return 1;
	}
	
	if(!port.empty())
	{
		// CCU2 Coprocessor
		pPortWrapper = new HM2Mod::CCU2SerialPortCommControllerMod();
		//Open connection to coprocessor via serial comport.
		if(!((HM2Mod::CCU2SerialPortCommControllerMod*)pPortWrapper)->init(port, true)) {
			return 3;
		}
	}
	else
	{
		LOG(Logger::LOG_ERROR, "Serial port not given.\n");
		return 2;
	}

	//Do the work
	bool done = startBootloader(pPortWrapper);
	if(done) {
		std::string response;
		std::string cmdData;
		LOG(Logger::LOG_INFO, "Starting speed test");
		LOG(Logger::LOG_INFO, "Retrieving %d times firmware version from coprocessor...", iterations);
		uint64_t* pIntermediates = new uint64_t[iterations];

		//Measure
		const uint64_t startTime = time_millis();
		for(int i = 0; i < iterations; i++) {
			pPortWrapper->sendSystemCommand(SYSTEMCMD_GETVERSION , cmdData, &response);
			pIntermediates[i] = time_millis();
		}
		const uint64_t endTime = time_millis();

		//Calculate and print out results
		LOG(Logger::LOG_INFO, "Speed test finished.");
		LOG(Logger::LOG_INFO, "Results:");
		const int64_t totalTime = endTime - startTime;
		LOG(Logger::LOG_INFO, "Total time needed: %d ms", totalTime);
		double variance = 0;
		double temp = 0;
		double minTime = DBL_MAX;
		double maxTime = DBL_MIN;
		const double meanTime = (double)totalTime / (double)iterations;
		for(int i = 0; i < iterations; i++) {
			double previous = 0;
			if(i >=1 ) {
				previous = pIntermediates[i-1] - startTime;
			}
			const double interval = (((double)(pIntermediates[i]-startTime)) - previous);
			if(verboseOutput) {
				LOG(Logger::LOG_INFO, "%d: %f", i, interval);
			}
			minTime = std::min(minTime, interval);
			maxTime = std::max(maxTime, interval);
			temp  = (interval - meanTime);
			temp = temp * temp;
			variance = variance + temp;
		}
		variance = variance / (double)iterations;
		const double deviation = sqrt(variance);
		LOG(Logger::LOG_INFO, "Mean time: %f ms", meanTime);
		LOG(Logger::LOG_INFO, "Variance: %f", variance);
		LOG(Logger::LOG_INFO, "Standard deviation: %f", deviation);
		LOG(Logger::LOG_INFO, "Minimum time: %d", (long)minTime);
		LOG(Logger::LOG_INFO, "Maximum time: %d\n", (long)maxTime);
		delete[] pIntermediates;
	}

	if(pPortWrapper != NULL) {
		delete pPortWrapper;
	}
	// Cleanup & return
	return 0;
}

void CoprocessorTest::printUsage()
{
	LOG(Logger::LOG_ERROR, "%s", help().c_str());
}


bool CoprocessorTest::startBootloader(HM2Mod::CCU2CommControllerMod* pPortWrapper)
{
	LOG(Logger::LOG_ALL, "CoprocessorTest::startBootloader()");
	std::string data;
	std::string response;

	if(pPortWrapper->startCoprocessorBootloader(true))
	{
		LOG(Logger::LOG_DEBUG, "CoprocessorTest::startBootloader():Coprocessor entered bootloader.");
	}
	else
	{
		LOG(Logger::LOG_ERROR, "CoprocessorTest::startBootloader():Could not start Coprocessor bootloader.\n");
			return false;
	}
	return true;
}


void CoprocessorTest::charArrayFromHexString(const std::string& hexStr, unsigned char** charArray, int& charArrayLength)
{
	unsigned int temp;
	int c = 0;
	if((hexStr.size() % 2) != 0) 
	{
		charArrayLength = 0;
		charArray = NULL;
		return;
	}
	
	charArrayLength = hexStr.size() / 2;
	*charArray = new unsigned char[charArrayLength];
	for(unsigned int i = 0; i < hexStr.size(); i+=2) 
	{
		temp = 0;
		std::stringstream ss;
		ss << std::hex << hexStr.substr(i, 2);
		ss >> temp;
		(*charArray)[c] = (unsigned char)(temp & 0xFF);
		c++;
	}
}

unsigned char CoprocessorTest::ConvertHexStringToByte(char high, char low)
{
	unsigned char highvalue;
	unsigned char lowvalue;
	unsigned char result;
	
	highvalue = ConvertHexCharToByte(high);
	lowvalue = ConvertHexCharToByte(low);	
	
	result = highvalue * 16 + lowvalue;
	return result;
}

unsigned char CoprocessorTest::ConvertHexCharToByte(const char value)
{
	unsigned char byte = value;
	if(value >= 'a')
	{
		byte = value - 'a' + 10;
	}
	else if(value >= 'A')
	{
		byte = value - 'A' + 10;
	}
	else if(value != ':')
	{	
		byte = value - '0';
	}
	
	return byte;
}

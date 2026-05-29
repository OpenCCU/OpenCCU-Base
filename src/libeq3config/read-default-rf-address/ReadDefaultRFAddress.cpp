/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#include "ReadDefaultRFAddress.h"

#include <iostream>
#include <iomanip>

#include <CCU2SerialPortCommControllerMod.h>
#include <CCU2CoprocessorCommandMod.h>
#include <HM2UtilsMod.h>
#include <sstream>

#include <ConsoleLogger.h>


using namespace std;
using namespace HM2Mod;


ReadDefaultRFAddress::ReadDefaultRFAddress()
: Command("read-default-rf-address") 
{
}


ReadDefaultRFAddress::~ReadDefaultRFAddress() {
}

int ReadDefaultRFAddress::execute() {
	const int paramsSize = static_cast<int>(params.size());
	bool printHex = false;
	bool verbose = false;
	std::string devPath;
	
		for(int i = 0; i < paramsSize ; i++) 
	{
		if((params.at(i).compare("-v") == 0)) {
			verbose = true;
		}
		else if((params.at(i).compare("-h") == 0)) {
			printHex = true;
		}
		else if((params.at(i).compare("-f") == 0) && ((i+1)<paramsSize)) {
			i++;
			devPath = params.at(i);
		}

	}
	
	//Check if params are valid
	if(devPath.empty()) {
		printUsage();
		return 1;
	}
	
	logger = new ConsoleLogger();		
	
	logger->SetLevel((Logger::LogLevel)5);

	/*CCU2SerialPortWrapperLinux pw;
	bool done = pw.Open(devPath);
	if(!done) {
		cout << "Cannot open serial port file " << devPath.c_str() << endl;
		return 1;
	}*/
	if(verbose) {
		cout << "Initalizing coprocessor..." << endl;
	}
	CCU2SerialPortCommControllerMod* commController = new CCU2SerialPortCommControllerMod();
	bool connected = commController->init(devPath);
	if(verbose) {
		cout << "Requesting default rf address" << endl;
	}
	std::string data;
	std::string params;
	if(commController->isDualCoprocessor()) {
		commController->sendLowLevelMacCommand(LOWLEVELMAC_COMMAND_GET_DEFAULT_RF_ADDRESS, &data);
	}
	else {
		commController->sendBidcosRequest(BIDCOSCMD_GET_DEFAULT_RF_ADDRESS, params, &data);
	}
	
	if(data.empty()) {
		cout << "Did not get an answer from coprocessor" << endl;
		return 1;
	}
	if(verbose) {
		cout << "Default RF-Address is: " << endl;		
	}
	//parse answer
	if(printHex) {

		stringstream ss;
		ss << hex << std::setfill('0');
		std::string out;
		ss << std::hex << std::setfill('0');
		for(unsigned int i = 0; i < data.size(); i++) {
			ss << std::setw(2) << (((unsigned int)data.at(i)) & 0x000000FF);
		}
		ss >> out;

		cout << "0x" << out.c_str() << endl;
	}
	else {
		int addrDec = convertBidEndianStringToBidcosAddress(data);
		cout << addrDec << endl;
	}

	return 0;

}

std::string ReadDefaultRFAddress::help() {
	std::string helpStr;
	helpStr.append("read-default-rf-address\n\n");
	helpStr.append("Reads the default RF address from HM MOD UART. Writes address to std out (decimal)\n");
	helpStr.append("Usage:\n\n");
	helpStr.append("readDefaultAddress [-v] [-h] -f <coprocessor-file>\n");
	helpStr.append("\t-v Be more verbose\n");
	helpStr.append("\t-h Print out hexadecimal instead of decimal\n");
	helpStr.append("\t-f <coprocessor-file> - Path to file representing serial interface of the HM MOD UART (i.e. /dev/ttyApp/\n");
	return helpStr;
}

void ReadDefaultRFAddress::printUsage() {
	cout << help().c_str() << endl;
}



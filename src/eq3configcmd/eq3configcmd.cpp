/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <string.h>

#include <CommandManager.h>
#include "generated-eq3configcmd-version.h"
#include "eq3config_version.h"

void printUsage();
void removeLeadingPathseperators(std::string& filename);

int main(int argc, char** argv)
{
	if(argc == 2 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
		std::cout << "eq3configcmd " EQ3CONFIGCMD_VERSION " (" __DATE__ ")" << std::endl
			<< "  libeq3config " << eq3config_version() << std::endl;
		return 0;
	}

	if(argc >= 1) {
		//Parse command line args...
		//First must be the commandName
		int argsOffset = 1;
		std::string commandName(argv[0]);

		if(commandName.find("eq3configcmd") != std::string::npos) {
			//commandName is name of the executable -> check if more parameters are given.
			if(argc >= 2) {
				commandName = argv[1];
				argsOffset = 2;
			}
			else {
				printUsage();
				return -2;
			}
		}
		//Remove leading stuff from commandName
		removeLeadingPathseperators(commandName);
		//	std::cout << "CommandName: " << commandName.c_str() << std::endl;
		
		Command* pCommand = CommandManager::getInstance()->getCommand(commandName);
		if(pCommand == NULL) {
			std::cout << "ERROR: unknown command \"" <<  commandName.c_str()  << "\"" << std::endl;
			std::vector<std::string> cmdNames = CommandManager::getInstance()->getCommandNames();
			std::cout << "Supported commands are:" << std::endl;
			for(unsigned int i = 0; i < cmdNames.size(); i++) {
				std::cout << cmdNames.at(i).c_str() << std::endl;
			}
			return -3;
		}
		//Set params
		pCommand->setParams(argc, argsOffset, argv);

		//Execute
		return pCommand->execute();
	}
	else {
		printUsage();
		return -1;
	}
	return 0;
}

void printUsage() {
	std::cout << std::endl
		<< "eq3configcmd " EQ3CONFIGCMD_VERSION " (" __DATE__ "), (c) 2013 eQ-3 Entwicklung GmbH" << std::endl
		<< std::endl
		<< "Usage:" << std::endl
		<< "\t eq3configcmd <command> [<args>]" << std::endl << std::endl
		<< "Try \"eq3configcmd help\" for additional information." << std::endl;
}

void removeLeadingPathseperators(std::string& filename)
{
	std::string::size_type index = filename.find_last_of('/');
	if(index != std::string::npos) {
		filename.assign(filename.substr(index+1));
	}
}

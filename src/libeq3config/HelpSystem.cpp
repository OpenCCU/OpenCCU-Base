/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HelpSystem.h"
#include <stdlib.h>
#include <stdio.h>
#include "CommandManager.h"

HelpSystem::HelpSystem() : Command("help")
{
}

HelpSystem::~HelpSystem()
{
}

int HelpSystem::execute()
{
	if(params.size() > 0) {
		//Try to print out commands help
		std::string commandName = params.at(0);
		removeLeadingPathseperators(commandName);
		Command* pCommand = CommandManager::getInstance()->getCommand(commandName);
		if(pCommand != NULL) {
			printf("%s", pCommand->help().c_str());
		}
		else {
			printf("Unknown command %s\n\n", commandName.c_str());
			printf("%s", help().c_str());	
		}

	}
	else {
		printf("%s", help().c_str());	
	}
	return 0;
}

std::string HelpSystem::help()
{
	std::string s("Usage: help <command>\nSupported commands are:\n");
	std::vector<std::string> cmdNames = CommandManager::getInstance()->getCommandNames();
	for(unsigned int i = 0; i < cmdNames.size(); i++) {
		s.append(cmdNames.at(i));
		s.append("\n");
	}
	return s;
}

void HelpSystem::removeLeadingPathseperators(std::string& filename)
{
	std::string::size_type index = filename.find_last_of('/');
	if(index != std::string::npos) {
		filename.assign(filename.substr(index+1));
	}
}

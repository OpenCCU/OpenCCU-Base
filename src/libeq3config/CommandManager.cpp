/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * CommandManager.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: user
 */

#include "CommandManager.h"

CommandManager::CommandManager()
{
}
/*
CommandManager::CommandManager(const CommandManager&)
{
}

CommandManager& CommandManager::operator=(const CommandManager&)
{
}
*/
CommandManager* CommandManager::getInstance()
{
	if (CommandManager::instance == NULL)
	{
		CommandManager::instance = new CommandManager();
	}

	return CommandManager::instance;
}

#include <stdio.h>
CommandManager* CommandManager::add(Command* command)
{
	commands.push_back(command);
	//printf("CommandManager::add(): Adding %s", command->GetName().c_str());
	return this;
}

const std::vector<Command*>& CommandManager::getCommands() const
{
	return commands;
}

Command* CommandManager::getCommand(const std::string& name) const
{
	for (int i = 0; i < commands.size(); i++)
	{
		Command* command = commands.at(i);
		if (name.compare(command->GetName()) == 0)
		{
			return command;
		}
	}

	return NULL;
}

std::vector<std::string> CommandManager::getCommandNames() const
{
	std::vector<std::string> commandNames;
	for (int i = 0; i < commands.size(); i++)
	{
		Command* command = commands.at(i);
		commandNames.push_back(command->GetName());
	}
	return commandNames;
}

CommandManager* CommandManager::instance = NULL;

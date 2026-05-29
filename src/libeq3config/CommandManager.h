/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * CommandManager.h
 *
 *  Created on: Mar 6, 2013
 *      Author: user
 */

#ifndef _COMMANDMANAGER_H_
#define _COMMANDMANAGER_H_

#include <vector>
#include <string>
#include "Command.h"


class CommandManager {
public:
	static CommandManager* getInstance();
	CommandManager* add(Command* command);
	const std::vector<Command*>& getCommands() const;
	Command* getCommand(const std::string& name) const;
	std::vector<std::string> getCommandNames() const;
private:
	CommandManager();
//	CommandManager(const CommandManager&);
//	CommandManager& operator=(const CommandManager&);

	static CommandManager* instance;
	std::vector<Command*> commands;
};

#endif /* COMMANDMANAGER_H_ */

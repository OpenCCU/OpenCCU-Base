/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#include "Command.h"
#include "CommandManager.h"

Command::Command(const std::string name)
: m_name(name)
{
	CommandManager::getInstance()->add(this);
}

Command::~Command() {
}

const std::string& Command::GetName() const
{
	return m_name;
}

void Command::setParams(const std::vector<std::string>& params)
{
	this->params = params;
}

void Command::setParams(const int argc, const int argoffset, char** const argv)
{
	for(int i = argoffset; i < argc; i++) {
		params.push_back(argv[i]);
	}
}

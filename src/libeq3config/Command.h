/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include <vector>

class Command {
public:
	Command(const std::string name);
	virtual ~Command();

	const std::string& GetName() const;
	void setParams(const int argc, const int argoffset, char** const argv);
	void setParams(const std::vector<std::string>& params);

	virtual int execute() = 0;
	virtual std::string help() = 0;

protected:
	std::string m_name;
	std::vector<std::string> params;

};

#endif /* COMMAND_H_ */

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HELPSYSTEM_H_
#define _HELPSYSTEM_H_

#include "Command.h"

class HelpSystem : public Command
{
public:
	HelpSystem();
	~HelpSystem();
	
	virtual int execute();
	virtual std::string help();
	
private:
	void removeLeadingPathseperators(std::string& filename);
};

#endif

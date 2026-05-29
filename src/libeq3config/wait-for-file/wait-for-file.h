/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _WAITFORFILE_H_
#define _WAITFORFILE_H_

#include <Command.h>


class WaitForFile : public Command
{
public:
	WaitForFile();

	virtual int execute();
	virtual std::string help();

private:
	void printUsage();
};

#endif /* _WAITFORFILE_H_ */

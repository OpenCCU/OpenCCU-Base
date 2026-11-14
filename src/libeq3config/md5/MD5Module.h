/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * helloworld.h
 *
 *  Created on: Mar 6, 2013
 *      Author: user
 */

#ifndef _MD5MODULE_H_
#define _MD5MODULE_H_

#include <Command.h>

class MD5Module : public Command
{
public:
	MD5Module();
	virtual ~MD5Module();

	virtual int execute();
	virtual std::string help();
private:
	std::string calculateMD5(const std::string& s);
};

#endif /* HELLOWORLD_H_ */

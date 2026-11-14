/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "crypttoolconfig.h"
#include <fstream>
#include <sstream>


// default constructor is not used
crypttoolconfig::crypttoolconfig(const std::string& configfilePath)
: configfilePath(configfilePath)
{
}

crypttoolconfig::crypttoolconfig()
: configfilePath("/etc/config/crypttool.cfg")
{
}

crypttoolconfig::~crypttoolconfig(void)
{
}


// Gets the last key from the config file
/*static*/ std::string crypttoolconfig::getCurrentKey(void)
{
	std::ifstream infile;
	infile.open(configfilePath.c_str(), std::ios_base::in);

	std::string key;
	int index = -1;

	if(infile.is_open())
	{
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);
			if (!(iss >> index >> key)) 
			{ 
				break; 
			}
		}

		infile.close();
	}

	return key;
}

// Gets the index from the config file
int crypttoolconfig::getCurrentKeyIndex(void)
{
	std::ifstream infile;
	infile.open(configfilePath.c_str(), std::ios_base::in);

	std::string key;
	int index = -1;

	if(infile.is_open())
	{
		std::string line;
		while (std::getline(infile, line))
		{
			std::istringstream iss(line);
			if (!(iss >> index >> key)) 
			{ 
				break; 
			}
		}

		infile.close();
	}

	return index;
}

// Adds the key to the config file
int crypttoolconfig::setCurrentKey(int index, std::string key)
{
	std::ofstream outfile;
	outfile.open(configfilePath.c_str(), std::ios_base::out | std::ios_base::app);
	if(outfile.is_open())
	{
		std::stringstream ss;
		ss << index << ' ' << key << std::endl;
		outfile << ss.str();
		outfile.close();
		return 0;
	}
	else
	{
		return 1;
	}
}

// deletes the content of the config file
int crypttoolconfig::resetConfig(void)
{
	std::ofstream outfile;
	outfile.open(configfilePath.c_str(), std::ios_base::out | std::ios_base::trunc);
	if(outfile.is_open())
	{
		outfile.close();
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HM2_FILEIOMOD_H_
#define _HM2_FILEIOMOD_H_

#include <string>

namespace HM2Mod {

class FileIOMod {
public:
	/** \brief Reads all characters from given file an returns them as string.
	* \details Uses character mode to read.
	* \return File content as string or empty string.
	*/
	static bool readStringFromFile(const std::string& filepath, std::string& result);

	/** \brief Writes given string to file.
	* \details Uses character mode. Overwrites the file.
	* \return True on success, otherwise False.
	*/
	static bool writeStringToFile(const std::string& filepath, const std::string& str);
};

}

#endif

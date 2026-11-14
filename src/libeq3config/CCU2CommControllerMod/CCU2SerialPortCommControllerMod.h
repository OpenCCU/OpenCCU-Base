/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2SERIALPORTCOMMCONTROLLERMOD_H_
#define _CCU2SERIALPORTCOMMCONTROLLERMOD_H_

#include <string>
#include <CCU2CoprocessorCommandMod.h>
#include <pthread.h>
#include <CCU2CommControllerMod.h>

namespace HM2Mod {

//Forward declarations (HM2Mod namespace)
class CCU2SerialPortWrapperMod;
class CCU2SerialFrameMod;
class CCU2BidcosRemoteInterfaceMod;

/** \brief This class is responsible for communication with the ccu 2 coprocessor.
* \details There are three types of communication with the ccu 2 coprocessor.
* Firstable coprocessor system commands, secondable coprocessor commands concerning bidcos and
* thirdly bidcos telegrams. Each of them is handled seperately and protected with an own mutex.
* Nevertheless there is only one receive thread to parse and handle incoming messages.
*/
class CCU2SerialPortCommControllerMod : public CCU2CommControllerMod
{
public:
	/** \brief Constructor.*/
	CCU2SerialPortCommControllerMod();
	/** \brief Destructor.*/
	virtual ~CCU2SerialPortCommControllerMod();
	
	/** \brief Initialization of the serial port connection.
	* \details Starts the coprocessor app-mode if needed.
	* \param dev Device file path.[in]
	* \param access Path of access file. [in]
	* \param reset Path of reset file. [in]
	*/
	bool init(const std::string& dev, bool skipStartApp = false);
};

}

#endif

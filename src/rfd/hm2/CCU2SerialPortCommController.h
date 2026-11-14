/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2SERIALPORTCOMMCONTROLLER_H_
#define _CCU2SERIALPORTCOMMCONTROLLER_H_

#include <string>
#include <CCU2CoprocessorCommand.h>
#include <pthread.h>
#include <CCU2CommController.h>

//Forward declarations (global namespace)
class BidcosFrame;

namespace HM2 {

//Forward declarations (HM2 namespace)
class CCU2SerialPortWrapper;
class CCU2SerialFrame;
class CCU2BidcosRemoteInterface;

/** \brief This class is responsible for communication with the ccu 2 coprocessor.
* \details There are three types of communication with the ccu 2 coprocessor.
* Firstable coprocessor system commands, secondable coprocessor commands concerning bidcos and
* thirdly bidcos telegrams. Each of them is handled seperately and protected with an own mutex.
* Nevertheless there is only one receive thread to parse and handle incoming messages.
*/
class CCU2SerialPortCommController : public CCU2CommController
{
public:
	/** \brief Constructor.*/
	CCU2SerialPortCommController(CCU2BidcosRemoteInterface* bidcosRemoteInterface);
	/** \brief Destructor.*/
	virtual ~CCU2SerialPortCommController();
	
	/** \brief Initialization of the serial port connection.
	* \details Starts the coprocessor app-mode if needed.
	* \param dev Device file path.[in]
	* \param access Path of access file. [in]
	* \param reset Path of reset file. [in]
	* \coprocessorSerialNr Serial number of coprocessor. [out]
	*/
	bool init(const std::string& dev, const std::string& access, const std::string& reset, const bool csmacaEnabled, const bool improvedCoproInit);
	

protected:



};

}

#endif

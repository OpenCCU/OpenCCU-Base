/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2SerialPortWrapper.h>

#ifndef WIN32
  #include <CCU2SerialPortWrapperLinux.h>
#else
  #include <CCU2SerialPortWrapperWin32.h>
#endif

using namespace HM2;


CCU2SerialPortWrapper::CCU2SerialPortWrapper() 
{
}

CCU2SerialPortWrapper::~CCU2SerialPortWrapper() 
{
}


CCU2SerialPortWrapper* CCU2SerialPortWrapper::createCCU2SerialPortWrapper() 
{
	#ifndef WIN32
	  return (CCU2SerialPortWrapper*)new CCU2SerialPortWrapperLinux();
	#else
      return (CCU2SerialPortWrapper*)new CCU2SerialPortWrapperWin32();
	#endif
}

void CCU2SerialPortWrapper::Disconnect() {
	Close();
}
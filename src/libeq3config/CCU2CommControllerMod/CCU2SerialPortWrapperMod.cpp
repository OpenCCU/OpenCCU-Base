/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2SerialPortWrapperMod.h>

#ifndef WIN32
  #include <CCU2SerialPortWrapperLinuxMod.h>
#else
  #include <CCU2SerialPortWrapperWin32Mod.h>
#endif

using namespace HM2Mod;


CCU2SerialPortWrapperMod::CCU2SerialPortWrapperMod()
{
}

CCU2SerialPortWrapperMod::~CCU2SerialPortWrapperMod()
{
}


CCU2SerialPortWrapperMod* CCU2SerialPortWrapperMod::createCCU2SerialPortWrapper()
{
	#ifndef WIN32
	  return (CCU2SerialPortWrapperMod*)new CCU2SerialPortWrapperLinuxMod();
	#else
      return (CCU2SerialPortWrapperMod*)new CCU2SerialPortWrapperWin32Mod();
	#endif
}

void CCU2SerialPortWrapperMod::Disconnect() {
	Close();
}


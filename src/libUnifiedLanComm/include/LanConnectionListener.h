/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LANCONNECTIONLISTENER_H_
#define _LANCONNECTIONLISTENER_H_

#include <DLLImportExportULC.h>

namespace ulc {

/** \brief Listener class for LanConnection.
 * \details Inheriting classes can (un)register a LanConnectionListener at LanConnection.*/
LIBUNIFIEDLANCOMM_API class LanConnectionListener {

public:
	virtual ~LanConnectionListener();
	virtual void onDisconnect() = 0;
};

}//namespace

#endif

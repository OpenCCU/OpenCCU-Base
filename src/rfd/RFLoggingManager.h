/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_LOGGING_MANAGER_H_
#define _RF_LOGGING_MANAGER_H_

#include "RFManager.h"
#include <vector>
#include <set>
//! Spezielle von RFManager abgeleitete Klasse, die empfangene Pakete einfach nur loggt
/*!
 *  Wird anstelle von RFManager instantiiert, wenn rfd im Logging-Modus gestartet wurde
 */
class RFLoggingManager:public RFManager
{
public:
	//! Konstruktor
	RFLoggingManager(void);
	//! Destruktor
	virtual ~RFLoggingManager(void);
	//! Verarbeitung (also nur loggen) eingehender Nachrichten
	virtual void ProcessIncomingFrame(BidcosFrame& msg);
	//! Minimale Initialisierung durchführen (nur Adresse an ARM7 übertragen)
	bool Init(const char* config_filename);
    //! Setzt einen Addressfilter. Falls gesetzt, werden nur die aufgelisteten Adressen geloggt.
    void SetAddressFilter( const std::vector<int>& addresses );
protected:
    //! Addressfilter. Falls gesetzt, werden nur die aufgelisteten Adressen geloggt.
    std::set<int> filtered_addresses;
};
#endif //_RF_LOGGING_MANAGER_H_

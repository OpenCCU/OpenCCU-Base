/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// BidcosLanInterface.h: Schnittstelle für die Klasse BidcosLanInterface.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BIDCOSLANINTERFACE_H_
#define _BIDCOSLANINTERFACE_H_

#include "BidcosLanConnection.h"
#include "BidcosRemoteInterface.h"

//! Spezialisierte Interfaceklasse für das Homematic-Lan-Interface

class BidcosLanInterface: public BidcosRemoteInterface
{
public:
	//! Konstruktor
	BidcosLanInterface();
	//! Destruktor
	virtual ~BidcosLanInterface();

    virtual bool Init(std::map<std::string, std::string>& params);

protected:
    //! Liefert das Verbindungsobjekt zurück
    BidcosInterfaceConnection* GetConnection();

private:
    //! Verbindungsobjekt
    BidcosLanConnection conn;
};

#endif // _BIDCOSLANINTERFACE_H_

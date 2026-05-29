/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSUSBINTERFACE_H_
#define _BIDCOSUSBINTERFACE_H_

#include "BidcosUsbConnection.h"
#include "BidcosRemoteInterface.h"

//! Spezialisierte Interfaceklasse für das Homematic-USB-Interface

class BidcosUsbInterface :
    public BidcosRemoteInterface
{
public:
	//! Konstruktor
    BidcosUsbInterface(void);
	//! Destruktor
    ~BidcosUsbInterface(void);
    virtual bool Init(std::map<std::string, std::string>& params);

protected:
    //! Liefert das Verbindungsobjekt zurück
    BidcosInterfaceConnection* GetConnection();

private:
    //! Verbindungsobjekt
    BidcosUsbConnection conn;
};
#endif

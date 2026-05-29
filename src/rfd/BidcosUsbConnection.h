/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSUSBCONNECTION_H_
#define _BIDCOSUSBCONNECTION_H_

#include "BidcosInterfaceConnection.h"

#include <map>
#include <string>
#include <queue>
#include <set>
#include "BidcosInterfaceMessage.h"
#include <HIDDevice.h>

//! Implementierung von BidcosInterfaceConnection für die Kommunikation mit Lan-Interfaces
class BidcosUsbConnection :
    public BidcosInterfaceConnection
{
public:
    //! Konstruktor
    BidcosUsbConnection(void);
    //! Destruktor
    virtual ~BidcosUsbConnection(void);
    virtual bool Init(std::map<std::string, std::string>& params);

    bool Connect(unsigned int* given_bidcos_address, unsigned int* native_bidcos_address, uint32_t* remote_timer);
    bool Disconnect();

    bool IsConnected();

    bool OutputMessageToInterface(BidcosInterfaceMessage& msg);
	bool RetrieveNextInputMessage(int timeout, BidcosInterfaceMessage* msg);

    std::string GetSerialNumber();

    bool CheckAvailable();

    bool HasError();
private:

    //! Füllt den Eingabequeue und wartet dabei maximal \c msTime ms auf Daten
	bool FillInputBuffer(int msTime);

    //! Konstanten
    enum {
        VENDOR_ID = 0x1B1F,//!< USB-Vendor-ID
        PRODUCT_ID = 0xC00F,//!<USB-Product-ID
        SET_REPORT_TIMEOUT = 2000//!<Timeout für das Senden eines HID-Reports
    };

    //! Seriennummer
  	std::string serial_number;

    //! Temporärpuffer für eingehende HID-Reports
    BYTE temp_input_buffer[65];
    //! Queue der empfangenen und noch nicht verarbeiteten Nachrichten
    std::queue<BidcosInterfaceMessage> queue_input_messages;
    //! Puffer für ausgehende HID-Reports
    BYTE output_buffer[65];

    //! Mutex für den Verbindungsauf- und abbau
	pthread_mutex_t mutex_connection;

    //! Mutex für Senden aus OutputMessageToSocket heraus
	pthread_mutex_t mutex_send;

    //! Kapselt die Kommunikation mit dem HID-Gerät
    CHIDDevice hiddev;

    //! Index des aktuellen Interfaces (normalerweise 0)
    unsigned int dev_index;

    //! Gibt an, ob ein Verbindungsfehler aufgetreten ist
    bool connection_error;
};

#endif

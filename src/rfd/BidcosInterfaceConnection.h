/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOS_INTERFACE_CONNECTION_H_
#define _BIDCOS_INTERFACE_CONNECTION_H_

#include <map>
#include <string>
#include "BidcosRemoteInterface.h"

//! Diese abstrakte Klasse kapselt die Art der Verbindung mit einem intelligenten Interface (z.B. Lan-Interface oder USB-Interface)
class BidcosInterfaceConnection
{
public:
    //! Konstruktor
    BidcosInterfaceConnection(void);
    //! Destruktor
    virtual ~BidcosInterfaceConnection(void);
    //! Initialisiert die Datenstrukturen des Verbindungsobjektes mit den Werten aus der Konfigurationsdatei
    /*!
     *  \param params Die Parameter der zum Interface gehörenden Sektion aus der Konfigurationsdatei
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    virtual bool Init(std::map<std::string, std::string>& params)=0;
    //! Gibt an, ob aktuell eine Verbindung zum physikalischen Interface besteht
    virtual bool IsConnected()=0;

    //! Stellt eine Verbindung her und liefert einige von der Gegenseite erhaltene Werte zurück
    /*!
     *  \param given_bidcos_address Zeiger auf Variable, die die dem Interface zuletzt zugeordnete Adresse zurückgibt
     *  \param native_bidcos_address Zeiger auf Variable, die die dem Interface in der Produktion zugeordnete Adresse zurückgibt
     *  \param remote_timer Zeiger auf Variable, die den aktuellen Stand des Betriebssystem-Timers des Interfaces zurückgibt
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    virtual bool Connect(unsigned int* given_bidcos_address, unsigned int* native_bidcos_address, uint32_t* remote_timer)=0;

    //! Beendet eine Verbindung
    /*!
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    virtual bool Disconnect()=0;
    //! Sendet eine Nachricht direkt an das Interface
    /*!
     *  \param msg Zu sendende Nachricht
     *  \param encrypt Bei \c true wird die Nachricht verschlüsselt gesendet
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     *
     *  Wird durch diese Methode ein Fehler angezeigt, ist die Verbindung zu schließen und neu aufzubauen.
     */
    virtual bool OutputMessageToInterface(BidcosInterfaceMessage& msg)=0;
    //! Liefert die nächste vom Interface gesendete Nachricht
    /*!
     *  \param timeout Maximale Wartezeit
     *  \param msg Zeiger auf die zurückgelieferte Nachricht
     *  \return \c bei true wurde eine Nachricht gelesen, bei \c false nicht
     *          \c false bedeutet entweder, dass der Timeout abgelaufen ist oder, dass ein Fehler
     *          aufgetreten ist. Der Fehlerfall kann mit HasError() abgefragt werden.
     */
   	virtual bool RetrieveNextInputMessage(int timeout, BidcosInterfaceMessage* msg)=0;
    //! Gibt zurück, ob ein Verbindungsfehler aufgetreten ist
    /*!
     *  Sollte aufgerufen werden, wenn RetrieveNextInputMessage() \c false zurückgibt. Wird durch diese
     *  Methode ein Fehler angezeigt, ist die Verbindung zu schließen und neu aufzubauen.
     */
    virtual bool HasError()=0;
	//! Informiert über Änderungen am Verbindungsstatus
	virtual void ReportConnectionStatus(const std::string& serial_number, bool status) const;
};

#endif

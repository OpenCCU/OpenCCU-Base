/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485Controller.h: Schnittstelle für die Klasse HS485Controller.
//
//////////////////////////////////////////////////////////////////////
#ifndef _HS485CONTROLLERCCU1_H_
#define _HS485CONTROLLERCCU1_H_


#include <map>
#include <string>
#include <vector>
//#include <CommController.h>
#include "CommMessageQueue.h"
//#include <XmlRpc.h>
#include "HS485CommMessage.h"
#include <HS485Controller.h>


//class HS485Controller;

//! Controller für die Kommunikation mit dem HS485-Kanal des Kommunikationsprozessors
/*! Diese Klasse erbt von CommController die grundlegenden Kommunikationsmechanismen.
 */
class HS485ControllerCCU1:public HS485Controller/*:public CommController, public XmlRpc::XmlRpcSource*/
{
protected:
	//! Speichert die Statusinformation für einen Kommunikationspartner (ein HS485-Gerät)
	/*! In dieser Klasse ist die Information über einen Kommunikationspartner abgelegt,
	 *  die die Klasse HS485Controller aus dem Sendethread und aus dem Empfangsthread heraus im
	 *  Zugriff haben muß.
	 */
/*	class AddressInfo
	{
	public:
		//! Konstruktor
		AddressInfo()
		{
			hw_address=0xffffffff;
			tx_counter=0xff;
			rx_counter=0xff;
		};
		//! Dem Objekt wurde bereits eine Adresse zugewiesen
		bool is_valid()
		{
			return hw_address!=0xffffffff;
		};
		//! HS485-Adresse des Kommunikationspartners
		uint32_t hw_address;
		//! Sendefolgezähler für die HS485-Kommunikation
		unsigned char tx_counter;
		//! Empfangsfolgezähler für die HS485-Kommunikation
		unsigned char rx_counter;
	};
*/
public:

	virtual HS485CommMessage* CreateNewMessage();

	//! Erstellt die Singleton Instanz.
	static HS485ControllerCCU1* CreateSingletonInstance();

	//! Gibt die Adresse der Zentrale am HS485-Bus zurück.
	/*! Die Adresse der Zentrale am HS485-Bus ist immer 1
	 */
	inline uint32_t GetAddress(){return 1;};

	//! Ermittelt für ein HS485-Gerät die Hardware-ID, Hardwareversion und Softwareversion
	/*! \return Binärer String aus 4 Zeichen:
	            - 1 Byte Hardware-ID
				- 1 Byte Hardware-Version
				- 2 Byte Softwareversion
	 */
    std::string GetDeviceDescription(uint32_t address);
	//! Sendet eine Nachricht in Form eines Objekts der Klasse HS485CommMessage auf den RS485-Bus
	/*! Bei Nachrichten, die eine Bestätigung erwarten, blockiert die Methode bis die Bestätigung empfangen wurde.
	 *  Bei Nachrichten, die keine Bestätigung erwarten (also auch Broadcast-Nachrichten), kehrt die Methode
	 *  sofort zurück.
	 *  \param msg Zeiger auf die zu sendende Nachricht
	 *  \return \c true bei erfolgreicher Sendung
	 */
	bool SendMessage(HS485CommMessage* msg);
	//! Sendet eine Nachricht in Form eines Strings auf den RS485-Bus
	/*! Es wird aus \c receiver und \c msg ein temporäres Objekt der Klasse HS485ComMessage erzeugt,
	 *  welches dann mittels der entsprechenden SendMessage()-Methode gesendet wird.
	 *  Eine Antwort wird aus der temporären Nachricht extrahiert und in \c response gespeichert.
	 *  \return \c true bei erfolgreicher Sendung
	 */
    bool SendMessage(uint32_t receiver, const std::string &msg, std::string *response);
	//! Sendet eine Bootloader-Nachricht in Form eines Strings auf den RS485-Bus
	/*! Es wird aus \c receiver und \c msg ein temporäres Objekt der Klasse HS485ComMessage erzeugt,
	 *  welches dann mittels der entsprechenden SendMessage()-Methode gesendet wird.
	 *  Eine Antwort wird aus der temporären Nachricht extrahiert und in \c response gespeichert.
	 *  \return \c true bei erfolgreicher Sendung
	 */
    bool SendBootloaderMessage(uint32_t receiver, const std::string &msg, std::string *response);
	//! Durchsucht den RS485-Bus nach Geräten
	/*! \param devices Zeiger auf einen Vector, der die Adressen der gefundenen Geräte aufnimmt
	 *  \return Anzahl der gefundenen Geräte. Im Fehlerfall \c -1
	 */
    int Discovery(std::vector<uint32_t>* devices);
	//! Löscht die Statusinformationen für einen Kommunikationspartner
	/*! Es wird die für den Kommunikationspartner mit der Adresse address die in HS485CommController
	 *  gespeicherte Statusinformation gelöscht.
	 *  Dies ist während eines Firmware-Updates nötig, wenn zwischen Bootloader und Applikation
	 *  umgeschaltet wird.
	 *  \param address Adresse der Kommunikationspartners oder \c 0xffffffff für alle Partner
	 */
	void ClearAddressInfo(uint32_t address=0xffffffff);
//	//! Versetzt per Broadcast alle Geräte am Bus in den Schlafzustand
//	/*! Im Schlafzustand führen die Geräte keine Aktionen aus und belegen insbesondere nicht den Bus.
//	 *  Um Kollisionen zu vermeiden werden während eines Firmware-Updates auf dem Bus sowie während des
//	 *  Suchens nach Geräten alle Geräte schlafen gelegt.
//	 *  \param on Bei \c true wird der Schlafzustand eingenommen, bei \c false wird dieser beendet
//	 */
//	void BroadcastSleepMode(bool on);

	//! Implementierung von XmlRpcSource::handleEvent
	/*! Diese Methode wird aus den XmlRpc Klassen heraus aufgerufen, wenn vom CommController eine
	 *  eingehende Nachricht empfangen wurde
	 */
    virtual unsigned handleEvent(unsigned eventType);


	//! Destruktor
	virtual ~HS485ControllerCCU1();


protected:

	//! Konstruktor
	HS485ControllerCCU1();

	//! Spezialisierung von NewMessage() für die Erzeugung von Objekten der Klasse HS485CommMessage
	inline virtual CommMessage* NewMessage();
	//! Führt unmittelbar vor dem Senden HS485-Spezifische Modifikationen an der Nachricht durch
	/*! Übernimmt die beiden Folgezähler aus dem zur Zieladresse gehörenden AddressInfo in den
	 *  HS485-Control-Character und inkrementiert den Sendefolgezähler im AddressInfo
	 */
	bool CheckBeforeSend(CommMessage* msg);
	//! Führt unmittelbar nach dem Empfang HS485-Spezifische Überprüfungen an der empfangenen Nachricht durch
	/*! Es wird der Empfangsfolgezähler aus der Nachricht extrahiert und in das entsprechende Objekt von
	 *  AddressInfo übernommen.
	 *  \retval \c false wenn eine Nachricht mit dem empfangenen Empfangsfolgezähler bereits empfangen wurde
	 *          \c true sonst.
	 */
	bool CheckAfterReceive(CommMessage* msg);
	//! Überprüfung des Empfangsfolgezählers einer eingehenden Nachricht
	/*! Wird aus CheckAfterReceive() heraus aufgerufen.
	 *  \retval \c false wenn eine Nachricht mit dem empfangenen Empfangsfolgezähler bereits empfangen wurde
	 *          \c true sonst.
	 */
	bool CheckRxCounter(uint32_t receiver, unsigned char cc);
	//! Setzt die beiden Folgezähler in einem Control-Character einer zu sendenden Nachricht
	/*! Wird aus CheckBeforeSend() heraus aufgerufen.
	 *  Übernimmt die beiden Folgezähler aus dem zur Zieladresse gehörenden AddressInfo in den
	 *  HS485-Control-Character und inkrementiert den Sendefolgezähler im AddressInfo
	 */
	void UpdateControlChar(uint32_t receiver, unsigned char* cc);
    //! Fügt eine empfangene Nachricht in die Empfangswarteschlange ein
   	void ProcessReceivedMessage(CommMessage* msg);
	//! Typedef für Map zur Verwaltung der Folgezähler für alle am RS485-Bus angeschlossenen Geräte
	typedef std::map<uint32_t, AddressInfo> t_map_address_info;
	//! Map zur Verwaltung der Folgezähler für alle am RS485-Bus angeschlossenen Geräte
	t_map_address_info map_address_info;
	//! Mutex für den Zugriff auf \c map_address_info
//	pthread_mutex_t mutex_address_info;
    //! Empfangswarteschlange
  //  CommMessageQueue rxq;
    //! Pipe zur Signalisierung eingehender Nachrichten
//	int pipe_fds[2];
};

#endif

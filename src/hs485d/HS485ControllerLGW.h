/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485CONTROLLERLGW_H_
#define _HS485CONTROLLERLGW_H_

#include <HS485Controller.h>
#include <pthread.h>

class HMWLGWTransportFrame;

class HS485ControllerLGW : public HS485Controller
{

	public:

	virtual ~HS485ControllerLGW();

	//! Gibt die Adresse der Zentrale am HS485-Bus zurück.
	/*! Die Adresse der Zentrale am HS485-Bus ist immer 1
	 */
	virtual inline uint32_t GetAddress(){return 1;};

	/** \brief Erstellt die Singleton Instanz.*/
	static HS485ControllerLGW* CreateSingletonInstance();

	//! Zugriff auf das einzige Objekt dieser Klasse
	/*! Aus Gründen der Vereinfachung wird dieses eine Objekt beim Start der
	 *  Applikation außerhalb dieser Klasse mit Hilfe der Methode CreateSingletonInstance explizit erzeugt.
	 */
	static HS485Controller* GetSingleton();
	//! Ermittelt für ein HS485-Gerät die Hardware-ID, Hardwareversion und Softwareversion
	/*! \return Binärer String aus 4 Zeichen:
	            - 1 Byte Hardware-ID
				- 1 Byte Hardware-Version
				- 2 Byte Softwareversion
	 */
    virtual std::string GetDeviceDescription(uint32_t address);
	//! Sendet eine Nachricht in Form eines Objekts der Klasse HS485CommMessage auf den RS485-Bus
	/*! Bei Nachrichten, die eine Bestätigung erwarten, blockiert die Methode bis die Bestätigung empfangen wurde.
	 *  Bei Nachrichten, die keine Bestätigung erwarten (also auch Broadcast-Nachrichten), kehrt die Methode
	 *  sofort zurück.
	 *  \param msg Zeiger auf die zu sendende Nachricht
	 *  \return \c true bei erfolgreicher Sendung
	 */
	virtual bool SendMessage(HS485CommMessage* msg);
	//! Sendet eine Nachricht in Form eines Strings auf den RS485-Bus
	/*! Es wird aus \c receiver und \c msg ein temporäres Objekt der Klasse HS485ComMessage erzeugt,
	 *  welches dann mittels der entsprechenden SendMessage()-Methode gesendet wird.
	 *  Eine Antwort wird aus der temporären Nachricht extrahiert und in \c response gespeichert.
	 *  \return \c true bei erfolgreicher Sendung
	 */
    virtual bool SendMessage(uint32_t receiver, const std::string &msg, std::string *response);
	//! Sendet eine Bootloader-Nachricht in Form eines Strings auf den RS485-Bus
	/*! Es wird aus \c receiver und \c msg ein temporäres Objekt der Klasse HS485ComMessage erzeugt,
	 *  welches dann mittels der entsprechenden SendMessage()-Methode gesendet wird.
	 *  Eine Antwort wird aus der temporären Nachricht extrahiert und in \c response gespeichert.
	 *  \return \c true bei erfolgreicher Sendung
	 */
    virtual bool SendBootloaderMessage(uint32_t receiver, const std::string &msg, std::string *response);
	//! Durchsucht den RS485-Bus nach Geräten
	/*! \param devices Zeiger auf einen Vector, der die Adressen der gefundenen Geräte aufnimmt
	 *  \return Anzahl der gefundenen Geräte. Im Fehlerfall \c -1
	 */
     virtual int Discovery(std::vector<uint32_t>* devices);
	//! Löscht die Statusinformationen für einen Kommunikationspartner
	/*! Es wird die für den Kommunikationspartner mit der Adresse address die in HS485CommController
	 *  gespeicherte Statusinformation gelöscht.
	 *  Dies ist während eines Firmware-Updates nötig, wenn zwischen Bootloader und Applikation
	 *  umgeschaltet wird.
	 *  \param address Adresse der Kommunikationspartners oder \c 0xffffffff für alle Partner
	 */
	virtual void ClearAddressInfo(uint32_t address=0xffffffff);
//	//! Versetzt per Broadcast alle Geräte am Bus in den Schlafzustand
//	/*! Im Schlafzustand führen die Geräte keine Aktionen aus und belegen insbesondere nicht den Bus.
//	 *  Um Kollisionen zu vermeiden werden während eines Firmware-Updates auf dem Bus sowie während des
//	 *  Suchens nach Geräten alle Geräte schlafen gelegt.
//	 *  \param on Bei \c true wird der Schlafzustand eingenommen, bei \c false wird dieser beendet
//	 */
	//virtual void BroadcastSleepMode(bool on);

	//! Implementierung von XmlRpcSource::handleEvent
	/*! Diese Methode wird aus den XmlRpc Klassen heraus aufgerufen, wenn vom CommController eine
	 *  eingehende Nachricht empfangen wurde
	 */
    virtual unsigned handleEvent(unsigned eventType);

    virtual HS485CommMessage* CreateNewMessage();

	virtual void BroadcastSleepMode(bool on);//Remove me later... not implemented in LGW right now

	/** \brief Processing of incoming raw data.
	* \details Overrides CommController::ProcessReceivedData().
	* This method parses raw data and assembles the message object.
	* \param s Data to process.
	* \return True as long as there is data in string s, which is not processed already. False if all data has been processed.
	*/
	virtual bool ProcessReceivedData(std::string* s);

//	/** \brief Starts receive/send threads.
//	 * \details Overrides CommController::Start()*/
//	virtual int Start();
//	/** \brief Stops receive/send threads.
//	 * \details Overrides CommController::Stop()*/
//	virtual int Stop();

	virtual bool getLGWStatus(std::string* serial, bool* connected);

protected:


	struct IncomingHMWLGWData {
		/** \brief HMWLGW Tranport frame currently build up.*/
		HMWLGWTransportFrame* pHMWLGWTransportFrame;
		/** \brief Leftover when a frame is complete and there is more data.*/
		std::string leftover;

		IncomingHMWLGWData();
		~IncomingHMWLGWData();
	};

	IncomingHMWLGWData incomingHMWLGWData;

	/** \brief Constructor.*/
	HS485ControllerLGW();

	/** \brief Creates a new CommMessage.
	 *	\details Inheriting classes must create the specialized CommMessage.
	 */
	inline virtual CommMessage* NewMessage();

	//! Führt unmittelbar vor dem Senden HS485-Spezifische Modifikationen an der Nachricht durch
	/*! Übernimmt die beiden Folgezähler aus dem zur Zieladresse gehörenden AddressInfo in den
	 *  HS485-Control-Character und inkrementiert den Sendefolgezähler im AddressInfo
	 */
	virtual bool CheckBeforeSend(CommMessage* msg);

	//! Führt unmittelbar nach dem Empfang HS485-Spezifische Überprüfungen an der empfangenen Nachricht durch
	/*! Es wird der Empfangsfolgezähler aus der Nachricht extrahiert und in das entsprechende Objekt von
	 *  AddressInfo übernommen.
	 *  \retval \c false wenn eine Nachricht mit dem empfangenen Empfangsfolgezähler bereits empfangen wurde
	 *          \c true sonst.
	 */
	virtual bool CheckAfterReceive(CommMessage* msg);
	//! Überprüfung des Empfangsfolgezählers einer eingehenden Nachricht
	/*! Wird aus CheckAfterReceive() heraus aufgerufen.
	 *  \retval \c false wenn eine Nachricht mit dem empfangenen Empfangsfolgezähler bereits empfangen wurde
	 *          \c true sonst.
	 */
	virtual bool CheckRxCounter(uint32_t receiver, unsigned char cc);
	//! Setzt die beiden Folgezähler in einem Control-Character einer zu sendenden Nachricht
	/*! Wird aus CheckBeforeSend() heraus aufgerufen.
	 *  Übernimmt die beiden Folgezähler aus dem zur Zieladresse gehörenden AddressInfo in den
	 *  HS485-Control-Character und inkrementiert den Sendefolgezähler im AddressInfo
	 */
	virtual void UpdateControlChar(uint32_t receiver, unsigned char* cc);
    //! Fügt eine empfangene Nachricht in die Empfangswarteschlange ein
   	virtual void ProcessReceivedMessage(CommMessage* msg);

   	virtual bool handleReceivedFrame(HMWLGWTransportFrame* pIncomingTransportFrame);

private:
	/** \brief Thread to send keep-alive messages.*/
	pthread_t keepaliveMsgThread;
	/** \brief Switch for keepaliveMsgThread. Set to false to stop keepaliveMsgThread*/
	volatile bool keepaliveMsgThreadEnabled;
	/** \brief Thread function of keepaliveMsgThread.*/
	static void* keepAliveMsgThreadFunction(void * pController);

};


#endif

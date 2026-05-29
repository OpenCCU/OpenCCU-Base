/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HM2BIDCOSREMOTEINTERFACE_H_
#define _HM2BIDCOSREMOTEINTERFACE_H_

#include <BidcosInterface.h>
#include <TripleBurstInterface.h>
#include <vector>

namespace HM2 {

class CCU2CommController;

class CCU2BidcosRemoteInterface :
	public BidcosInterface, public ITripleBurstInterface
{
public:

	enum InterfaceSubType {
		CCU2,			//CCU 2 coprocessor (CCU 2 internal)
		HMLGW2,			//CCU 2 coprocessor on HM-LGW-O-TW-W-EU
		HM_MOD_UART		//HM-MOD-UART, using the same communication protocol as the CCU 2 coprocessor
	};

	CCU2BidcosRemoteInterface(const InterfaceSubType& ifSubtype);
	virtual ~CCU2BidcosRemoteInterface();

    //! Sets temporary key
    /*!
     *  \param index Index of new temporary key
     *  \param data 16 Byte long key. MD5-Hash of the key/passphrase the user has entered.
     *  \return \c true on success, \c false on error
     */
	virtual bool SetAesKeyTemp(int index, const std::string& data)/*=0*/;
    //! Setzt den aktuellen und vorherigen Benutzerschl�ssel
    /*!
     *  \param index Index des aktuellen Benutzerschl�ssels
     *  \param data 16 Byte langer aktueller Benutzerschl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \param last_index Index des vorherigen Benutzerschl�ssels
     *  \param last_data 16 Byte langer vorheriger Benutzerschl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data)/*=0*/;
    //! Startet das Interface
    /*!
     *  \param bidcos_address Bidcos-Funk-Adresse mit der das Interface sendet
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool StartInterface(int bidcos_address)/*=0*/;
    //! Stoppt das Interface
	virtual bool StopInterface()/*=0*/;
    //! Sendet einen Bidcos-Rahmen und wartet falls n�tig auf eine Antwort
    /*!
     *  \param frame Der zu sendende Rahmen
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     *
     *  Falls es sich um einen bidirektionalen Rahmen handelt, wird auf die Antwort gewartet und diese
     *  in \c frame als Antwort gespeichert. Es wird nur die direkt auf den gesendeten Rahmen empfangene
     *  Antwort gespeichert. Um das Sammeln der weitere Antworten in einer l�nger dauernden Kommunikation 
     *  k�mmert sich die Klasse \c BidcosInterfaceConcentrator
     */
	virtual bool SendFrame(BidcosFrame* frame)/*=0*/;

	//Sendet einen Frame mit Triple Burst (Überschreibt ITripleBurstInterface::SendFrameTripleBurst())
	virtual bool SendFrameTripleBurst(BidcosFrame* frame)/*=0*/;

    //! Ger�t hinzuf�gen
    /*!
     *  Diese Methode f�gt ein Ger�t zum Interface hinzu. Das Interface ist damit f�r die Kommunikation
     *  mit diesem Ger�t verantwortlich. Von dem Ger�t empfangene Rahmen werden vom Interface in Zukunft
     *  automatisch best�tigt.
     *  \param address Bidcos-Adresse des hinzuzuf�genden Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool AddDevice(int address);
    //! Ger�t entfernen
    /*!
     *  Diese Methode entfernt ein Ger�t aus dem Verantwortungsbereich des Interfaces. Von dem Ger�t empfangene 
     *  Rahmen werden vom Interface in Zukunft nicht mehr automatisch best�tigt.
     *  \param address Bidcos-Adresse des zu entfernenden Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool RemoveDevice(int address);
    //! Setzt den AES-Schl�ssel und die gesicherten Kan�le f�r ein Ger�t 
    /*
     *  \param address Bidcos-Adresse des Ger�tes, dessen AES-Regel modifiziert werden soll
     *  \param aes_key Index des vom Ger�t aktuell verwendeten AES-Schl�ssels
     *  \param aes_channels Bitmaske, die angibt, f�r welche Kan�le des Ger�tes AES aktiviert ist
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels);
    //! F�gt eine Aufweckanforderung f�r ein Ger�t hinzu
    /*!
     *  Nach dem Aufruf dieser Methode wird das Interface versuchen, das entsprechende Ger�t aufzuwecken.
     *  Das Aufwecken eines Ger�tes geht immer mit dem Empfang eines Rahmens von dem Ger�t einher. Wenn
     *  das Ger�t aufgeweckt wurde, gibt BidcosFrame::DeviceWokenup() des empfangenen Rahmens \c true zur�ck.
     *
     *  \param address Bidcos-Adresse des Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool AddDeviceWakeupRequest(int address,bool lazyConfig = false);
    //! Entfernt eine Aufweckanforderung f�r ein Ger�t
    /*!
     *  Nach dem Aufruf dieser Methode wird das Interface nicht mehr versuchen, das entsprechende Ger�t aufzuwecken.
     *
     *  \param address Bidcos-Adresse des Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool RemoveDeviceWakeupRequest(int address);

    virtual bool Init(std::map<std::string, std::string>& params);
    //! Gibt an, ob aktuell eine Verbindung zum physikalischen Interface besteht
    virtual bool IsConnected()/*=0*/;
    //! Gibt die in der Produktion vergebene und die in der Installation gesetzte Adresse des Interfaces zur�ck
    /*!
     *  Wird beim ersten Start des Bidcos-Service aufgerufen, um eine eindeutige Bidcos-Adresse f�r den 
     *  Bidcos-Service zu ermitteln. Der Aufruf dieser Methode erfolgt vor dem Aufruf von Start().
     *  \param native_address Zeiger auf Variable, in der die produktionsseitig vergebene Adresse des Interfaces
     *         zur�ckgeliefert wird.
     *  \param given_address Zeiger auf Variable, in der eine zuvor gesetzte Adresse des Interfaces zur�ckgegeben wird.
     *         Gibt 0 zur�ck, wenn noch keine Adresse gesetzt wurde.
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    virtual bool DonateAddress(unsigned int* native_address, unsigned int* given_address);

	virtual bool SupportsTripleBurst();

//	void handleEvent(const std::string& bidcosEventFrameData);
	void handleEvent(BidcosFrame bidcosFrame);

	/** \brief Handles a duty cycle event from CCU2-CoProcessor.
	 * \param dutyCycleValue Current duty cycle value as double. Values between 0.0 and 1.0.
	*/
	void handleDutyCycleEvent(const double dutyCycleValue);

	/** \brief Calls method in BidcosInterface to inform about firmware version.
	* \param firmwareVersion The firmware version: Major.Minor.Revision */
	void publishFirmwareVersion(const std::string& firmwareVersion);
	
	/*! Setzt die UTC Zeit f�r das Interface.
	* \param utcSeconds Anzahl Sekunden seit 01.01.1970 00:00 Uhr (UTC)
	* \param offsetMinutes Offset in Minuten entsprechend der Zeitzone.
	* \return Gibt false zur�ck bei einem Fehler, ansonsten true (Default-Implementierung).*/
	virtual bool SetInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes);

	/** \brief Publishes all internally known devices to ccu2 coprocessor.
	 * \details This method is used to re-add all known devices to coprocessor in case
	 * that the coprocessor application was RESTARTED. App->BL->App.
	 * This method does NOT come from BidCoSInterface.
	 * \return True on success, otherwise false.*/
	virtual bool republishAllDevices();
/*! Setzt die Datenrate des Interface auf 100kBit/s 
	* \ return true: die Datenrate wurde auf 100kBit/s gestzt andernfalls false */
	virtual bool Set100kMode();
	/*! Setzt die Datenrate des Interface auf 10kBit/s 
	* \ return true: die Datenrate wurde auf 10kBit/s gestzt andernfalls false */
	virtual bool Set10kMode();
	/*! Gibt die eingestellte Datenrate zur�ck
	* \ return akktuelle Datenrate */
	virtual DataRate_t getDataRate();
	/*! Gibt auskunft ob das Interface in der Lage ist ein Ger�t �ber Funk zu aktualisieren.
	 * \ return true: das Interface kann ein Upade durchf�hren, andernfalls wird false zur�ckgegeben */
	virtual bool Updateable();
	/*! Gibt auskunft ob das Interface LazyConfig unterst�tzt.
	 * \ return true: das Interface unterst�tzt LazyConfig, andernfalls wird false zur�ckgegeben */
	virtual bool SupportLazyConfig();
	virtual int GetDutyCycle() const;

	/** \brief Sets info LED of HomeMatic RF-Lan Gateway (rfd internal type: HMLGW2)
     * param state 0: off; 1: on; 2: blink slow (1 second); 3: blink fast (500ms)
     */
	virtual bool SetRFLGWInfoLED(const unsigned int state);

	/** \brief Sets coprocessor time by system time.
	* \details Used to set initial time on initialization and re-initialization.
	*/
	void SetInterfaceClockBySystemTime();

private:

	//! Constants for AES keys (Copied from BidcosRemoteInterface.h)
	enum AesKeyType{
		AES_KEY_TYPE_DEFAULT=0, //!< default key
		AES_KEY_TYPE_CURRENT_USER, //!< Active user key
		AES_KEY_TYPE_PREVIOUS_USER, //!< Previous key
		AES_KEY_TYPE_TEMP, //!< Temporary key
	};

	CCU2CommController* pCommController;
	DataRate_t dataRate;

	class AESKeyData {
	public:
		int currentKeyIndex;
		std::string currentKey;
		int previousKeyIndex;
		std::string previousKey;
	};

	AESKeyData aesKeys;
	
	/** \brief Helper method for Init() that initializes the CCU2SerialPortCommController*/
	bool InitCCU2SerialPortCommController(std::map<std::string, std::string>& params, bool improvedCoproInit);
	/** \brief Helper method for Init() that initializes the CCU2LGWCommController*/
	bool InitHMLGWPortCommController(std::map<std::string, std::string>& params, bool improvedCoproInit);

	/** \brief Sets given aes key internally*/
//	bool SetAESKeyInternally(AesKeyType type, int index, const std::string &key);
	/** \brief Sets given aes key remotely*/
	bool SetAESKeyRemote(AesKeyType type, int index, const std::string &key);

	bool AddDeviceRemote(int address, std::string* authChannels = NULL);

	void setupAuthChannelNrDistribution(const uint64& bidcosAuthChannels, const uint64& deviceAuthChannels, std::vector<int>& activeChannels, std::vector<int>& inactiveChannels);
	
};

}//namespace HM2

#endif

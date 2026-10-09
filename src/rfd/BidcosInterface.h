/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSINTERFACE_H_
#define _BIDCOSINTERFACE_H_

#include <typedefs.h>
#include <pthread.h>
#include <map>
#include <string>

class BidcosInterfaceConcentrator;
class BidcosFrame;

//! Abstrakte Basisklasse f�r Funk-Interface-Ger�te
/*!
 *  Diese Klasse definiert die Schnittstelle, die von BidcosInterfaceConcentrator verwendet wird, um
 *  Funk-Interfaces anzusprechen.
 */
class BidcosInterface
{
public:
	typedef enum DataRate_e
	{
		DATA_RATE_10k = 10,
		DATA_RATE_100k = 100,
	}DataRate_t;
    //! Konstruktor
	BidcosInterface(void);
    //! Destruktor
	virtual ~BidcosInterface(void);

    //! Liefert die Indizes der derzeit im Funk-Interface gespeicherten AES-Schl�ssel zur�ck
    /*!
     *  \param default_key Zeiger auf eine Variable, in der der Index des Standardschl�ssels zur�ckgeliefert wird
     *  \param current_kex Zeiger auf eine Variable, in der der Index des aktuellen Benutzerschl�ssels zur�ckgeliefert wird
     *  \param previous_key  Zeiger auf eine Variable, in der der Index des vorherigen Benutzerschl�ssels zur�ckgeliefert wird
     *  \param temp_key Zeiger auf eine Variable, in der der Index des Tempor�rschl�ssels zur�ckgeliefert wird
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool GetAesKeyIndexes(int* default_key, int* current_key, int* previous_key, int* temp_key){return false;};
    //! Setzt den Tempor�rschl�ssel
    /*!
     *  \param index Index des neuen Tempor�rschl�ssels
     *  \param data 16 Byte langer Schl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool SetAesKeyTemp(int index, const std::string& data)=0;
    //! Setzt den aktuellen und vorherigen Benutzerschl�ssel
    /*!
     *  \param index Index des aktuellen Benutzerschl�ssels
     *  \param data 16 Byte langer aktueller Benutzerschl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \param last_index Index des vorherigen Benutzerschl�ssels
     *  \param last_data 16 Byte langer vorheriger Benutzerschl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data)=0;
    //! Startet das Interface
    /*!
     *  \param bidcos_address Bidcos-Funk-Adresse mit der das Interface sendet
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	virtual bool StartInterface(int bidcos_address)=0;
    //! Stoppt das Interface
	virtual bool StopInterface()=0;
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
	virtual bool SendFrame(BidcosFrame* frame)=0;

    //! Setzt die Instanz der Klasse BidcosInterfaceConcentrator, an die empfangene Rahmen gesendet werden.
	void SetConcentrator(BidcosInterfaceConcentrator* ic);

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

    //! Liefert die Seriennummer des Interfaces zur�ck
	const std::string& GetSerialNumber();
    //! Liefert die Beschreibung des Interfaces aus der Konfigurationsdatei des Bidcos-Service zur�ck
	const std::string& GetDescription();
	//! Liefert den Typ des Interfaces
	const std::string& GetInterfaceType() const;
	//! Ermittelt, ob das Interface per Netzwerk angebunden ist (Funk-LAN-Gateway)
	/*! Der Typ stammt aus dem "Type"-Eintrag der Interface-Sektion der Konfigurationsdatei */
	bool IsLanInterface() const;
	//! Liefert die Firmware-Version des Interfaces
	const std::string& GetFirmwareVersion() const;
    //! Initialisiert die Datenstrukturen des Interface-Objektes mit den Werten aus der Konfigurationsdatei
    /*!
     *  \param params Die Parameter der zum Interface geh�renden Sektion aus der Konfigurationsdatei
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    virtual bool Init(std::map<std::string, std::string>& params);
    //! Gibt an, ob aktuell eine Verbindung zum physikalischen Interface besteht
    virtual bool IsConnected()=0;
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
    virtual bool DonateAddress(unsigned int* native_address, unsigned int* given_address){return false;};

    //! Erzeugt eine neue Instanz einer von BidcosInterface abgeleiteten Klasse aus einem Typ-String
    /*!
     *  \param type Aus der Konfigurationsdatei stammender Interfacetyp. M�gliche Werte:
     *              - "CCU" erzeugt ein Objekt der Klasse RFController
     *              - "Lan Interface" erzeugt ein Objekt der Klasse BidcosLanInterface
     *              - "USB Interface" erzeugt ein Objekt der Klasse BidcosUsbInterface
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    static BidcosInterface* CreateFromType(const std::string& type);

	//! Liefert die aktuelle DutyCycle-Auslastung
	virtual int GetDutyCycle() const;

	/*! Setzt die UTC Zeit f�r das Interface.
	* \param utcSeconds Anzahl Sekunden seit 01.01.1970 00:00 Uhr (UTC)
	* \param offsetMinutes Offset in Minuten entsprechend der Zeitzone.
	* \return Gibt false zur�ck bei einem Fehler, ansonsten true (Default-Implementierung).*/
	virtual bool SetInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes);
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
    /** \brief Sets info LED of HomeMatic RF-Lan Gateway (rfd internal type: HMLGW2)
     * param state 0: off; 1: on; 2: blink slow (1 second); 3: blink fast (500ms)
     */
	virtual bool SetRFLGWInfoLED(const unsigned int state);

	//! Ermittelt, ob das Interface Triple Burst unterstützt. (Derzeit nur bei Coprozessoren auf CCU 2 Basis der Fall)
	virtual bool SupportsTripleBurst();
protected:
    //! Typedef f�r die dem Interface zugeordneten Ger�te
    typedef std::vector<int> t_device_list;
    //! Liefert den zugeordneten BidcosInterfaceConcentrator zur�ck
	BidcosInterfaceConcentrator* GetConcentrator();
    //! Liefert die AES-Regel f�r ein Ger�t zur�ck
	bool GetDeviceAesPolicy(int address, int* aes_key, uint64* channels);
    //! Liefert die dem Interface zugeordneten Ger�te zur�ck
    void ListDevices(t_device_list* devices);
    //! Liefert die aufzuweckenden Ger�te zur�ck
    void ListWakeupDevices(t_device_list* devices);
    //! Ermittelt, ob ein bestimmtes Ger�t aufgeweckt wedren soll
    bool NeedsWakeup(int address);
    //! Ermittelt, ob ein bestimmtes Ger�t mit LazyConfig angesprochen werden soll
    bool NeedsLazyConfig(int address);
	//! Setzt die Firmware-Version des Interfaces
	void SetFirmwareVersion(const std::string& firmware_version);
	//! Setzt DutyCycle; verschickt bei Bedarf Ereignisse (Wertebereich 0-100)
	void SetDutyCycle(int new_duty_cycle);

private:
	//! Liefert die unter Grenze der aktuellen Duty-Cycle-Schwelle
	int GetDutyCycleLowerBound() const;
	//! Liefert die obere Grenze der aktuellen Duty-Cycle-Schwelle
	int GetDutyCycleUpperBound() const;
	//! Aktualisiert die Duty-Cycle-Schwelle
	void UpdateDutyCycleThreshold(int new_duty_cycle);
	//! Zeiger auf den zugeordneten BidcosInterfaceConcentrator
	BidcosInterfaceConcentrator* concentrator;
    //! Seriennummer des Interfaces
	std::string serial_number;
    //! Beschreibung des Interfaces
    std::string description;
	//! Typ des Interfaces
	std::string iface_type;
	//! Firmware-Version des Interfaces;
	std::string firmware_version;
	//! Aktuelle Duty-Cycle-Auslastung
	int duty_cycle;
	//! Aktuelle Duty-Cycle Schwelle
	int duty_cycle_threshold;
	
    //! Klasse f�r die von einem BidcosInterface f�r ein Ger�t gespeicherten Informationen
	class DeviceData
	{
	public:
        //! Flags
		enum{
			FLAG_WAKEUP=1, //!< Ger�t soll aufgeweckt werden
			FLAG_LAZY_CONFIG = 2 //! Ger�t soll �ber den LazyConfig Mechanismuss angesprochen werden
		};
        //! Konstruktor
		DeviceData();
        //! Destruktor
		~DeviceData();
        //! Bidcos-Adresse
		int address;
        //! Bitfeld f�r Flags
		int flags;
        //! Index des aktuellen AES-Schl�ssels
		int aes_key;
        //! Bitfeld der Kan�le des Ger�tes, f�r die AES aktiviert ist
		uint64 aes_channels;
	};

    //! Typedef f�r Map Adresse -> DeviceData der zugeordneten Ger�te
	typedef std::map<int, DeviceData> t_map_devices;
    //! Map Adresse -> DeviceData der zugeordneten Ger�te
	t_map_devices map_devices;
	//! Mutex f�r den multithreaded Zugriff auf map_devices
	pthread_mutex_t mutex_devices;
};

#endif

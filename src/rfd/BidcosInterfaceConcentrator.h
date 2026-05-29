/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOSINTERFACECONCENTRATOR_H_
#define _BIDCOSINTERFACECONCENTRATOR_H_

#include <string>
#include <map>
#include <deque>
#include <XmlRpc.h>
#include <typedefs.h>
#include "BidcosFrame.h"
#include <pthread.h>
#include <TimerTarget.h>
#include <PropertyMap.h>
#include <UpdateFile.h>
#include <set>


class BidcosInterface;
class RFDevice;

//! Diese Klasse verwaltet mehrere Funk-Interfaces in Form von Instanzen von von BidcosInterface abgeleiteten Klassen
/*!
 *  Ausgehende Rahmen werden gem�� der aktuellen Zuordnung zwischen Ger�ten und Interfaces verteilt.
 *  Eingehende Rahmen werden nur weiterverarbeitet, wenn sie vom dem Ger�t zugeordneten Interface empfangen wurden.
 *  Die Empfangsfeldst�rke aller eingehender Rahmen wird ausgewertet, um bei f�r ein Ger�t aktiviertem Roaming
 *  die Zuordnung dynamisch anpassen zu k�nnen.
 */
class BidcosInterfaceConcentrator: public XmlRpc::XmlRpcSource, TimerTarget
{
public:
    //! Konstruktor
	BidcosInterfaceConcentrator(void);
    //! Destruktor
	~BidcosInterfaceConcentrator(void);
    //! Erzeugt alle in der Konfigurationsdatei spezifizierten Interfaces
    /*!
     *  \param config Konfigurationsdatei in Form einer PropertyMap
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    bool CreateInterfacesFromFile(PropertyMap& config);
    //! Ermittelt beim ersten Start eine eindeutige Adresse f�r den Bidcos-Service
    /*!
     *  \param address Zeigt auf Variable, in der die ermittelte Adresse zur�ckgeliefert wird
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    bool BegInterfacesForAddress(unsigned int* address);
    //! Wird von den Interface-Objekten aufgerufen, wenn ein Rahmen empfangen wurde.
    /*!
     *  Diese Methode wird im Kontext von verschiedenen Threads aufgerufen und ist daher threadsicher.
     *  \param frame Der empfangene Rahmen
     */
	bool ProcessReceivedFrame(const BidcosFrame& frame);
    //! Sendet einen Rahmen
    /*!
     *  Wartet bei einem bidirektionalen Rahmen, bis dieser gesendet wurde. Kehrt bei einem unidirektionalen
     *  Rahmen sofort zur�ck.
     *  \param frame Der zu sendende Rahmen
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool SendFrame(BidcosFrame* frame);

	/** \brief Sendet einen Multicast (Broadcast an Adresse des ersten Empf�ngers + Unicast an Rest)
	* \details Diese Methode wird bislang nur f�r die virtuelle Fernbedienung (HM-RCV-50) verwendet. (05.07.2013)
	* \param peer_devices Ger�te an die der Multicast gesendet wird.
	* \param frame Zu sendender Rahmen.
	* \param burst Wenn True, wird zumindest das erste Telegramm per Burst verschickt.
	* \return True im Erfolgsfall. False, wenn zumindest ein Adressat nicht geantwortet hat.
	*/
	bool PerformMulticastSend(std::set<RFDevice*>& peer_devices, BidcosFrame* frame, bool burst);

    //! Setzt die Bidcos-Adresse des Bidcos-Service
    /*!
     *  Muss vor StartInterfaces() aufgerufen werden.
     *  \param address Die zu setzende Adresse
     */
	void SetBidcosAddress(int address);
    //! Liefert die Bidcos-Adresse des Bidcos-Service zur�ck
	int GetBidcosAddress();

    //! Schaltet den promiskuitiven Modus, in dem alle empfangenen Rahmen weitergereicht werden, ein oder aus
    /*!
     *  Der promiskuitive Modus wird nur im Sniffer-Betrieb aktiviert.
     */
    void SetPromicuousMode(bool pm);

    //! Ermittelt die aktuellen AES-Schl�ssel von den Interfaces
    /*!
     *  Wird beim Start aufgerufen, wenn die Persistierung von Schl�sseln ausgeschaltet ist. Dann werden die Schl�ssel
     *  im physikalischen Interface gespeichert. Dies ist auf der CCU der Fall.
     *  \param current_kex Zeiger auf eine Variable, in der der Index des aktuellen Benutzerschl�ssels zur�ckgeliefert wird
     *  \param previous_key  Zeiger auf eine Variable, in der der Index des vorherigen Benutzerschl�ssels zur�ckgeliefert wird
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool BegInterfacesForKeys(int* current_key, int* previous_key);
    //! Setzt den Tempor�rschl�ssel
    /*!
     *  \param index Index des neuen Tempor�rschl�ssels
     *  \param data 16 Byte langer Schl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool SetAesKeyTemp(int index, const std::string& data);
    //! Setzt den aktuellen und vorherigen Benutzerschl�ssel
    /*!
     *  \param index Index des aktuellen Benutzerschl�ssels
     *  \param data 16 Byte langer aktueller Benutzerschl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \param last_index Index des vorherigen Benutzerschl�ssels
     *  \param last_data 16 Byte langer vorheriger Benutzerschl�ssel. MD5-Hash des vom Benutzer eingegebenen Schl�ssels
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data);
    //! Startet alle Interfaces
    /*!
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool StartInterfaces();
    //! Stoppt alle Interfaces
    /*!
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool StopInterfaces();

    //! Ger�t hinzuf�gen und einem Interface zuordnen
    /*!
     *  Diese Methode f�gt ein Ger�t dem BidcosInterfaceConcentrator hinzu und ordnet dies einem Interface zu.
     *  Falls das Ger�t vorher einem anderen Interface zugeordnet war, wird diese Zuordnung aufgehoben.
     *
     *  \param address Bidcos-Adresse des zuzuordnenden Ger�tes
     *  \param current_interface Seriennummer des Interfaces, dem das Ger�t zzugeordnet werden soll
     *  \param roaming_allowed Bei \c true wird die Interfacezuordnung des Ger�tes automatisch angepasst
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool AddDevice(int address, const std::string& current_interface, bool roaming_allowed);

	//! Gerät initial hinzufügen und einem Interface zuordnen. Muss nach Erstellung der Geräteinstanz aufgerufen werden (anstatt obige Methode)
	bool AddDevice(RFDevice* device, int address, const std::string& current_interface, bool roaming_allowed);

    //! Ger�t entfernen
    /*!
     *  Diese Methode entfernt ein Ger�t aus dem BidcosInterfaceConcentrator.
     *
     *  \param address Bidcos-Adresse des Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool RemoveDevice(int address);
    //! Setzt den AES-Schl�ssel und die gesicherten Kan�le f�r ein Ger�t 
    /*
     *  \param address Bidcos-Adresse des Ger�tes, dessen AES-Regel modifiziert werden soll
     *  \param aes_key Index des vom Ger�t aktuell verwendeten AES-Schl�ssels
     *  \param aes_channels Bitmaske, die angibt, f�r welche Kan�le des Ger�tes AES aktiviert ist
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels);
    //! F�gt eine Aufweckanforderung f�r ein Ger�t hinzu
    /*!
     *  Nach dem Aufruf dieser Methode wird das zugeordnete Interface versuchen, das entsprechende Ger�t aufzuwecken.
     *  Das Aufwecken eines Ger�tes geht immer mit dem Empfang eines Rahmens von dem Ger�t einher. Wenn
     *  das Ger�t aufgeweckt wurde, gibt BidcosFrame::DeviceWokenup() des empfangenen Rahmens \c true zur�ck.
     *
     *  \param address Bidcos-Adresse des Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool AddDeviceWakeupRequest(int address,bool lazyConfig = false);
    //! Entfernt eine Aufweckanforderung f�r ein Ger�t
    /*!
     *  Nach dem Aufruf dieser Methode wird das zugeordnete Interface nicht mehr versuchen, das entsprechende 
     *  Ger�t aufzuwecken.
     *
     *  \param address Bidcos-Adresse des Ger�tes
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
	bool RemoveDeviceWakeupRequest(int address);
    //! Liefert alle Interfaces zur�ck
    /*!
     *  \param vec_interfaces Zeiger auf Vektor, in dem die Zeiger auf die BidcosInterface-Objekte zur�ckgeliefert werden
     *  \param default_interface Zeiger auf Variable, in der Zeiger auf das Standardinterface zur�ckgeliefert wird
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    bool ListInterfaces(std::vector<BidcosInterface*>* vec_interfaces, BidcosInterface** default_interface);

	//! Implementierung von XmlRpcSource::handleEvent
	/*! Diese Methode wird aus den XmlRpc Klassen heraus aufgerufen, wenn vom CommController eine
	 *  eingehende Nachricht empfangen wurde
	 */
    unsigned handleEvent(unsigned eventType);

    //! Implementierung von TimerTarget::OnTimer()
    /*!
     *  F�hrt zeitgesteuerte Aktionen bzgl. Roaming durch
     */
    void OnTimer(uint32_t cookie);
    //! Liefert die Seriennummer des Standardinterfaces zur�ck
    std::string GetDefaultInterfaceId();
	bool IsInterfaceUpdatable(int address);
	bool SetInterfaceTo100kMode(int address);
	bool SetInterfaceTo10kMode(int address);
	bool SendUpdataFrame(const std::string &uFrame, int deviceAddress);
	bool DutyCycleForUpdate(int address,UpdateFile &uFile);
private:
    //! Klasse f�r die von einem BidcosConcentrator f�r ein Ger�t gespeicherten Informationen
	class DeviceData
	{
	public:
        //! Flags
		enum{
			FLAG_WAKEUP=1, //!< Ger�t soll aufgeweckt werden
			FLAG_ROAMING=2, //!< Roaming ist f�r das Ger�t aktiviert
            FLAG_TEMPORARY=4, //!< Es handelt sich um ein tempor�res Ger�t w�hrend des Anlernvorganges
            FLAG_ROAM_INHIBIT=8, //!< Roaming ist f�r das Ger�t zeitweilig nicht erlaubt
            FLAG_LAZY_CONFIG=16, //!< Ger�t soll mit LazyConfig konfiguriert werden
			FLAG_TRIPLE_BURST=32 //!< Ger�t unterst�tzt Triple Burst anstatt Single Burst
		};
        //! Konstruktor
		DeviceData();
        //! Destruktor
		~DeviceData();
        //! Liefert den Sendefolgez�her f�r das Ger�t zur�ck
		int GetTxCounter();
		//! Liefert den Sendefolgez�hler f�r das Ger�t zur�ck ohne ihn zu inkrementieren. ACHTUNG: Nnu
		int GetTxCounterWithoutIncrement();
		//! ACHTUNG: NUR F�R SONDERF�LLE: �berschreibt den Sendefolgez�hler f�r das Ger�t. 
		void SetTxCounter(const int counter);
        //! Aktualisiert den Sendefolgez�hler f�r das Ger�t
        /*!
         *  \param received_counter Empfangener Sendefolgez�hler
         */
		void UpdateTxCounter(int received_counter);
        //! Bidcos-Adresse des Ger�tes
		int address;
        //! Seriennummer des zugeordneten Interfaces
        std::string cur_interface_id;
        //! Bitfeld f�r Flags
		int flags;
        //! Index des vom Ger�t verwendeten AES-Schl�ssels
		int aes_key;
        //! Empfangsfolgez�hler
		int rx_counter;
        //! Sendefolgez�hler
		int tx_counter;
        //! Zeitstempel in ms f�r die G�ltigkeit des Flags \c FLAG_ROAM_INHIBIT
        unsigned int roam_inhibit_valid_until;
        //! Bitfeld der Kan�le des Ger�tes, f�r die AES aktiviert ist
		uint64 aes_channels;
        //! Kopie des letzten vom Ger�t empfangenen Rahmens
		BidcosFrame last_received_frame;
	};

    //! F�gt ein Interface hinzu
	bool AddInterface(BidcosInterface* bi);
    //! Entfernt ein Interface
	bool RemoveInterface(const std::string& serial);
    //! Entfernt ein Ger�t vom aktuell Interface DeviceData::cur_interface_id
	bool RemoveDeviceFromInterface(DeviceData& dev_data);
    //! F�gt ein Ger�t zum Interface DeviceData::cur_interface_id hinzu
	bool AddDeviceToInterface(DeviceData& dev_data);
    //! Setzt ein neues Standardinterface, falls das alte nicht mehr existiert oder es noch keines gab
	void UpdateDefaultInterface();
    //! Liefert aus einer Seriennummer einen Zeiger auf ein Interface-Objekt
    BidcosInterface* GetInterface(const std::string& interface_id);

    //! Setzt das Interface aus DeviceData::last_received_frame als neues Interface
    bool CheckAndUpdateInterfaceAssociation(DeviceData& dev_data);

	//! Interne Methode zum Senden eines Frames �ber das jeweilige Interface.
	bool SendFrame(BidcosFrame*	frame, BidcosInterface* iface, const int receiver, uint64_t& waitTime);

	enum{
		RX_QUEUE_MAX_SIZE=1024 //!< Gr��e der Queue f�r empfangenen Rahmen
	};
    enum{
        ROAMING_CHECK_DELAY=150, //!< Zeitpunkt in ms nach dem Empfang eines Rahmens, zu dem die Interfacezuordnung �berpr�ft wird
        AFTER_SEND_ROAM_INHIBIT_TIME=300 //!< Zeitspanne in ms nach dem Senden eines Rahmens in der kein Roaming statt findet
    };
    enum{
        //! Timer-ID f�r die �berpr�fung der Interfacezuordnung. Die unteren drei Byte enthalten die Bidcos-Adresse des Ger�tes.
        TIMER_ID_CHECK_ROAMING=0x01000000 
    };
    //! Typedef f�r Interface-Map
	typedef std::map<std::string, BidcosInterface*> t_map_interfaces;
    //! Map Seriennummer -> Interface der vorhandenen Interfaces
	t_map_interfaces map_interfaces;

    //! Zeiger auf das Standardinterface
	BidcosInterface* default_interface;

    //! Typedef f�r Map adresse -> DeviceData der angelernten Ger�te
	typedef std::map<int, DeviceData> t_map_devices;
    //! Map adresse -> DeviceData der angelernten Ger�te
	t_map_devices map_devices;

    //! Bidcos-Adresse des Bidcos-Service
	int bidcos_address;

    //! Pipe-Dateideskriptoren f�r Einbindung in XmlRpc::XmlRpcDispatch
	int pipe_fds[2];

    //! Zeiger auf den aktuell auf Antworten wartenden Rahmen
    BidcosFrame* cur_tx_frame;

    //! Empfangsqueue
	std::deque<BidcosFrame> rx_frame_queue;
	//! Mutex f�r den multithreaded Zugriff auf die Empfangsqueue
	pthread_mutex_t mutex_rx_frame_queue;
	//! Mutex f�r den multithreaded Zugriff auf die Ger�tedaten
	pthread_mutex_t mutex_devices;
    //! Mutex f�r den Zugriff auf den aktuell gesendeten Frame
    pthread_mutex_t mutex_cur_tx_frame;
	//! Condition f�r die Signalisierung zwischen mehreren Threads
	pthread_cond_t cond_cur_tx_frame;
    //! Bei true werden keine Frames von unbekannten Ger�ten zur�ckgehalten
    bool promiscuous_mode;
};

#endif

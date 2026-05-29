/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HSS_MANAGER_H_
#define _HSS_MANAGER_H_

#include "dllexport.h"

#include <map>
#include <XmlRpc.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif 
#include <PropertyMap.h>
#include "MetadataStore.h"
#include "LogicalInstance.h"
#include "HSSXmlRpcEventDispatcher.h"

#ifdef ReportEvent
#undef ReportEvent
#endif


//! Abstrakte Basisklasse f�r die zentralen Manager-Klassen der einzelnen Applikationen
/*! In jeder Interface-Applikation gibt es �blicherweise ein zentrales Objekt
 *  einer von HSSManager abgeleiteten Klasse. Diese szentrale Objekt ist verantwortlich f�r
 *  die Verwaltung der angelernten Ger�te und f�r die Kommunikation nach au�en per 
 *  XmlRpc (Logikschicht) und UDP (Displayprozess).
 *  F�r eingehende XmlRpc-Kommunikation bietet sie Methoden an, die von den XmlRpc-Methoden-Objekten
 *  aufgerufen werden. F�r ausgehende XmlRpc-Kommunikation bietet sie den abgeleiteten Klassen
 *  die Methoden XmlRpcCallSync und XmlRpcCallAsync an.
 *  HSSManager kapselt den von den abgeleiteten Manager-Klassen gemeinsam verwendeten Code.
 */
class DLLEXPORT HSSManager
{
public:
	//! Enum f�r den Parameter \c hint der XmlRpc-Methode updateDevice
	enum{UPDATE_HINT_ALL=0, UPDATE_HINT_LINKS=1};
	//! Konstruktor
	HSSManager(void);
	//! Destruktor
	virtual ~HSSManager(void);
	//! Wird von anderen Objekten aufgerufen, um ein Ereignis, dass als Servicenachricht markiert ist mitzuteilen
	/*! HSSManager speichert die aktiven Servicenachrichten in einer Map. Diese Methode aktualisiert diese Map
	 *  und sendet ein UDP-Paket mit der Anzahl aktiver Servicemeldungen an den Display-Prozess.
	 *  Der Aufruf von ReportServiceMessage() ersetzt nicht den Aufruf von ReportEvent()
	 *  \param address Adresse des Kanals, dem die Servicenachricht zugeordnet ist
	 *  \param value_key Parameter-ID der Servicenachricht
	 *  \param val Wert des Parameters f�r die Servicenachricht. Es werden Werte von Typ \c bool und \c int
	 *             akzeptiert. Dabei stehen die Werte \c false bzw. \c 0 f�r "Servicemeldung nicht aktiv".
	 */
	void ReportServiceMessage(const std::string& address, const std::string& value_key, XmlRpc::XmlRpcValue& val);
	//! Wird von anderen Objekten aufgerufen, um ein Ereignis, mitzuteilen
	/*! HSSManager verteilt �ber diese Methode mitgeteilte Ereignisse per XmlRpc an alle registrierten Logikbl�cke
	 *  \param address Adresse des Kanals, dem die Servicenachricht zugeordnet ist
	 *  \param value_key Parameter-ID der Servicenachricht
	 *  \param val Wert des Parameters f�r das Ereignis
	 */
	void ReportEvent(const std::string& address, const std::string& value_key, XmlRpc::XmlRpcValue& val);
	//! Mu� von abgeleiteten Klassen implementiert werden und ein XmlRpc-Array von Ger�tebeschreibungen liefern
	/*! \param devs Zeiger auf ein XmlRpc-Array, an das die Ger�tebeschreibungen angeh�ngt werden. Aufbau gem��
	 *              Dokumentation zu listDevices() der XmlRpc-Schnittstelle
	 */
	virtual bool ListDevices(XmlRpc::XmlRpcValue* devs)=0;
	//TODO: Beschreibung
	bool CommitReplacedDevices(const std::string url,const std::string& id);
	virtual bool IsDeviceReplaced(const std::string &oldDeviceAddress, std::string &newDeviceAddress)=0;
	//! Wird bei der Registrierung eines Logikprozesses �ber die XmlRpc-Schnittstelle aufgerufen
	/*! Entspricht der Methode init() der XmlRpc-Schnittstelle
	 *  \param url URL des XmlRpc-Servers an den Ereignisse gesendet werden sollen
	 *  \param id Vom Logikprozess vergebener Cookie, der dem Logikprozess mit jedem Ereignis geschickt wird. 
	 *            Hier�ber kann der Logikprozess mehrere Schnittstellenprozesse unterscheiden.
	 *  \param dont_callback Wenn true werden im Kontext von PlatformInit keine XmlRpc-Methoden \c ListDevices
	 *            \c NewDevices und \c DeleteDevices aufgerufen.
	 */
	bool PlatformInit(const std::string& url, const std::string& id, bool dont_callback=false);
	//! Liefert zu einer Adresse das logische Objekt (Kanal oder Ger�t) zur�ck.
	/*! Diese Methode mu� von der abgeleiteten Klasse implementiert werden. Wird dort normalerweise mit
	 *  spezifischerem R�ckgabetyp neu deklariert.
	 */
	virtual LogicalInstance* GetInstance(const std::string& address)=0;
	//! Sendet eine Nachricht per UDP-Port 8182 an den Displayprozess
	/*! Es wird ein UDP-Paket mit Textuellem Inhalt gesendet. In der ersten Zeile steht der Wert der Variablen 
	 *  \c task_id.
	 *  In der zweiten Zeile stehen die Parameter \c key und \c value in der Form key=value.
	 *  \param key Schl�ssel f�r die zweite Zeile
	 *  \param value Wert f�r die zweite Zeile
	 *  \param timer Wenn \c timer!=0 wird kein UDP-Paket gesendet, wenn die letzte Sendung f�r den gleichen Schl�ssel
	 *               weniger als timer ms zur�ckliegt.
	 */
	void SendUDPInfo(const std::string& key, const std::string& value="", uint64_t timer=0);
	//! Startet das Sammeln von XmlRpc-Aufrufen per system.multicall
	/*! Alle Aufrufe an XmlRpcCallAsync() werden zwischengespeichert und erst beim Aufruf von MulticallCollectEnd()
	 *  gesammelt per system.multicall abgeschickt.
	 *  Dadurch wird f�r zeitgleich eintreffende Ereignisse (z.B. 5 verschiedene Messwerte von einem Wettersensor)
	 *  nur eine Kommunikation per XmlRpc ben�tigt.
	 */
	void MulticallCollectBegin(){multicall++;};
	//! Beendet das Sammeln von XmlRpc-Aufrufen per system.multicall
	/*! Sendet alle zwischengespeicherten Aufrufe an XmlRpcCallAsync() per system.multicall.
	 *  Dies passiert erst, wenn die Anzahl der Aufrufe von MulticallCollectEnd() der Anzahl der Aufrufe von 
	 *  MulticallCollectBegin() entspricht.
	 */
	void MulticallCollectEnd();
	//! Liefert alle gespeicherten Servicenachrichten zur�ck
	/*! \param messages XmlRpc-Array, das die Nachrichten aufnimmt
     *     Jedes Feld im Array ist wiederum ein Array bestehend aus drei Elementen:
     *     - Kanaladresse des zur Servicemeldung geh�renden Kanals
     *     - Werte-ID der Servicemeldung
     *     - Wert der Servicemeldung
	 */
    void GetServiceMessages(XmlRpc::XmlRpcValue* messages);
	//! Initialisierungsmethode. Wird beim Starten des Prozesses aufgerufen
	virtual bool Init(const char* config_filename);
    //! Liefert die Konfigurationsdatei als PropertyMap zur�ck
    PropertyMap& GetConfigPropertyMap();
    //! Setzt ein Metadatum zu einem Objekt
	/*! \param object_id Identifier f�r ein Objekt. In der Regel eine Ger�te- oder Kanalseriennummer
     *  \param data_id Identifier f�r das gew�nschte Metadatum
     *  \param value Zu setzender Wert
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 */
    bool MetadataSet(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue& value);
    //! Liefert ein Metadatum zu einem Objekt zur�ck
	/*! \param object_id Identifier f�r ein Objekt. In der Regel eine Ger�te- oder Kanalseriennummer
     *  \param data_id Identifier f�r das gew�nschte Metadatum
     *  \param value Zeiger auf die Variable, die den abgefragten Wert aufnimmt
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 */
    bool MetadataGet(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue* value);
    //! Liefert alle Metadaten zu einem Objekt zur�ck
	/*! \param object_id Identifier f�r ein Objekt. In der Regel eine Ger�te- oder Kanalseriennummer
     *  \param value Zeiger auf ein XmlRpc-Struct, das die abgefragten Werte aufnimmt
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 */
    bool MetadataGetAll(const char* object_id, XmlRpc::XmlRpcValue* value);
    //! L�scht alle Metadaten zu einem Objekt
	/*! \param object_id Identifier f�r ein Objekt. In der Regel eine Ger�te- oder Kanalseriennummer
     *         Ein Asterisk "*" am Ende ist als Wildcard erlaubt, z.B. um zu einem Ger�t auch alle 
     *         Kanal-Metadaten zu l�schen.
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 */
    bool MetadataDelete(const char* object_id);
		//! Liefert fl�chtige Metadaten
		bool MetadataGetVolatile(const char* data_id, XmlRpc::XmlRpcValue* value);
		//! Setzt fl�chtige Metadaten
		bool MetadataSetVolatile(const char* data_id, XmlRpc::XmlRpcValue& value);
		//! L�scht fl�chtige Metadaten
		bool MetadataDeleteVolatile(const char* data_id);

	/** \brief Generates a pong event.
	 * \details event(CENTRAL_SERIAL, "pong", callerId)
	 * Gives registered clients the possibility to ping rfd/hs485d
	 * and check if it's registered for events.
	 */
	void Ping(XmlRpc::XmlRpcValue& callerId);

protected:
	//! Sendet alle gespeicherten Servicenachrichten per XmlRpc an einen Logikprozess
	/*! \param url URL des Logikprozesses, an den gesendet werden soll
	 *  \param id Cookie, den der Logikprozess beim Aufruf von PlatformInit() �bergeben hat
	 */
	void SendServiceEvents(const std::string& url, const std::string& id);
	//! Entfernt die gespeicherten Servicenachrichten f�r nicht mehr vorhandene Ger�te und sendet die Anzahl an den Displayprozess
	/*! Stellt sicher, dass alle gespeicherten Servicenachrichten zu noch vorhandenen Ger�ten geh�ren. Zum Schlu� wird die
	 *  neu ermittelte Anzahl per UDP an den Displayprozess gesendet.
	 *  Diese Methode wird jedesmal aufgerufen, wenn ein Ger�t gel�scht wird, um die Service-Anzeige der Zentrale zu
	 *  synchronisieren.
	 */
	void ValidateServiceMessages();
	//! L�dt die Liste der registrierten Logikprozesse
	/*! Die registrierten Logikprozesse werden aus der Datei /var/&lt;task_id&gt;.handlers geladen.
	 *  Diese Methode wird w�hrend der Initialisierung einer abgeleiteten Klasse beim Systemstart aufgerufen.
	 *  Sie dient dem einfacheren Debuggen. Die Datei mit den Logikprozessen solle in der Ramdisk liegen.
	 */
	bool LoadXmlRpcHandlers();
	//! Speichert die Liste der registrierten Logikprozesse
	/*! Die registrierten Logikprozesse werden in die Datei /var/&lt;task_id&gt;.handlers gespeichert.
	 *  Diese Methode wird aufgerufen, sobald sich ein neuer Logikprozess registriert hat.
	 */
	bool SaveXmlRpcHandlers();
	//! Hilfsfunktion f�r einen synchronen XmlRpc-Aufruf
	/*! \param method Die aufzurufene XmlRpc-Methode
	 *  \param params Die zu �bergebenen Parameter
	 *  \param result Zeiger auf die Variable, die die XmlRpc-Antwort aufnimmt
	 *  \param url URL des aufzurufenden XmlRpc-Servers
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 *  Im Fehlerfall werden Logmeldungen ausgegeben.
	 */
	bool XmlRpcCallSync(const char* method, XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue* result, const std::string& url);
	//! Hilfsfunktion f�r einen asynchronen XmlRpc-Aufruf
	/*! Diese Methode startet einen neuen Thread, um einen XmlRpc-Aufruf durchzuf�hren.
	 *  Sie kehrt sofort zur�ck, ohne abzuwarten, ob der Aufruf erfolgreich war. Einen Fehler bei der
	 *  XmlRpc-Kommunikation ersieht man hierbei nur aus dem Logfile.
	 *  Wenn das Zwischenspeichern von XmlRpc-Aufrufen gerade aktiv ist, wird einfach StoreMulticall()
	 *  aufgerufen und dann sofort zur�ckgekehrt.
	 *  \param method Die aufzurufene XmlRpc-Methode
	 *  \param params Die zu �bergebenen Parameter
	 *  \param url URL des aufzurufenden XmlRpc-Servers, "" f�r alle registrierten Logikprozesse
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 */
	bool XmlRpcCallAsync(const char* method, XmlRpc::XmlRpcValue& params, const std::string& url="");
	//! Speichert einen  f�r einen asynchronen XmlRpc-Aufruf
	/*! Diese Methode startet einen neuen Thread, um einen XmlRpc-Aufruf durchzuf�hren.
	 *  Sie kehrt sofort zur�ck, ohne abzuwarten, ob der Aufruf erfolgreich war. Einen Fehler bei der
	 *  XmlRpc-Kommunikation ersieht man hierbei nur aus dem Logfile.
	 *  Wenn das Zwischenspeichern von XmlRpc-Aufrufen gerade aktiv ist, wird einfach StoreMulticall()
	 *  aufgerufen und dann sofort zur�ckgekehrt.
	 *  \param method Die aufzurufene XmlRpc-Methode
	 *  \param params Die zu �bergebenen Parameter
	 *  \param url URL des aufzurufenden XmlRpc-Servers, "" f�r alle registrierten Logikprozesse
	 *  \return \c false im Fehlerfall, \c true im Erfolgsfall
	 */
	bool StoreMulticall(const char* method, XmlRpc::XmlRpcValue& params, const std::string& url="");
	//! Typedef f�r die Speicherung von Servicenachrichten
	typedef std::map<std::string, XmlRpc::XmlRpcValue> t_service_message_map;
	//! Map f�r die Speicherung von Servicenachrichten
	t_service_message_map service_message_map;
	//! Map f�r die Speicherung und Verwaltung der Zeitstempel f�r SendUDPInfo()
	std::map<std::string, uint64_t> udp_timer_map;
	//! Socketdescriptor f�r den UDP-Socket zum Senden. Wird im Konstruktor initialisiert.
	int udp_fd;
	//! Von der abgeleiteten Klasse zu setzender Identifier f�r die Anwendung. Z.B. "RFD" oder "HS485D"
	std::string task_id;
	//! f�r die UDP-Kommunikation verwendete Struktur f�r die Zieladresse
    struct sockaddr_in udp_dest_addr;
	//! Typedef f�r die registrierten Logikprozesse
	typedef std::map<std::string, HSSXmlRpcEventDispatcher*> t_handler_map;
	//! Map der registrierten Logikprozesse. URL->ID
	t_handler_map xmlrpc_handlers;
	//! Typedef f�r die Zwischenspeicherung von XmlRpc-Aufrufen
	typedef std::map<std::string, XmlRpc::XmlRpcValue> t_multicall_map;
	//! Map f�r die Zwischenspeicherung von XmlRpc-Aufrufen
	t_multicall_map multicall_map;
	//! Z�hler f�r das Aktivieren und Deaktivieren der Multicall-Zwischenspeicherung
	/*! Wird von MulticallCollectBegin() inkrementiert und von MulticallCollectEnd() dekrementiert
	 */
	unsigned int multicall;
    //! Konfigurationsdatei
    PropertyMap config_file;
    //! Objekt zur Speicherung von objektbezogenen Metadaten
    MetadataStore metadata_store;

	/** \brief Specifies if unreachable clients are removed from client list or not.
	 * \details Default: True. Can be switched offs in rfd.conf with 'Remove Unreachable Clients = false'
	 */
	bool removeUnreachableClients;

	std::string xmlrpcHandlersFilepath;
};
#endif

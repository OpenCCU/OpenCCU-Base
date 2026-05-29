/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSParamset.h: Schnittstelle f�r die Klasse HSSParamset.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPARAMSET_H_
#define _HSSPARAMSET_H_

#include "dllexport.h"


#include <string>
#include <map>
#include <XmlRpc.h>

#include "xmlParser.h"
#include "HSSParameter.h"
#include "LogicalInstance.h"

//! Abstrakte Basisklasse der Zusammenfassung einer Menge von Objekten der Klasse HSSParameter
/*! Parameter werden f�r die XmlRpc-Schnittstelle zu Parametersets zusammengefasst. Innerhalb des
 *  Parametersets werden die Parameter �ber Ihre ID angesprochen.
 *  Es gibt an der XmlRpc-Schnittstelle 3 verschiedene Arten von Parametersets:
 *  - \c MASTER ist ein Parameterset f�r (BidCoS-)Konfigurationsdaten, die sich auf einen Kanal oder
 *    ein Ger�t beziehen.
 *    Beispielparameter: Zeitschwelle f�r langen Tastendruck einer Fernbedienung
 *  - \c LINK ist ein Parameterset, das sich auf einen Kanal bezieht und die Konfigurationsparameter
 *    f�r die Verkn�pfung zu einem konkreten anderen Kanal (Partnet f�r direkte Kommunikation) sammelt.
 *    Beispielparameter: Einschaltverz�gerung eines Schaltaktors in der Verkn�pfung mit einer bestimmten
 *    Fernbedienungstaste
 *  - \c VALUES ist ein Parameterset, das sich auf einen Kanal bezieht und die Zustandsinformation f�r
 *    den Kanal sammelt.
 *    Beispielparameter: Schaltzustand eines Schaltaktors
 */
class DLLEXPORT HSSParamset  
{
public:
	//! Konstruktor
	HSSParamset();
	//! Destruktor
	virtual ~HSSParamset();
	//! Ruft f�r jeden Paraeter die SetDefaultConfig Methode auf 
	virtual bool SetDefaultConfig(LogicalInstance *inst);
	//! Erzeugt die an der XmlRpc-Schnittstelle ben�tigte Parameterset-Beschreibung
	/*! Diese Methode l�uft �ber alle sichtbaren Parameter und ruft HSSParameter::GetDescription() auf.
	 *  Die Beschreibung jedes Parameters wird mit der jeweiligen ID als Schl�ssel in dem XmlRpc-Struct
	 *  \c *set abgelegt.
	 *  Entspricht dem XmlRpc-Aufruf \c GetParamsetDescription()
	 */
    virtual bool GetDefinition(XmlRpc::XmlRpcValue *set);
	//! Fragt die Werte aller Parameter des Parametersets ab
	/*! Der Wert jedes Parameters wird mit der jeweiligen ID als Schl�ssel in dem XmlRpc-Struct
	 *  \c *set abgelegt.
	 *  Entspricht dem XmlRpc-Aufruf \c GetParamset()
	 *  \param inst Ger�te- oder Kanalobjekt, auf das sich die Abfrage bezieht
	 *  \param peer Verkn�pfungspartner auf den sich die Abfrage bezieht [Nur bei \c LINKS relevant]
	 *  \param set Zeiger auf die Variable, die die Schl�ssel-Werte-Paare aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool Get(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue *set, int mode = 0);
	//! Setzt die Werte mehrerer Parameter des Parametersets
	/*! Der Wert jedes zu setzenden Parameters wird mit der jeweiligen ID als Schl�ssel in dem XmlRpc-Struct
	 *  \c set erwartet.
	 *  Nicht in \c set enthaltene Parameter werden ignoriert
	 *  Entspricht dem XmlRpc-Aufruf \c PutParamset()
	 *  \param inst Ger�te- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param peer Verkn�pfungspartner auf den sich die Aktion bezieht [Nur bei \c LINKS relevant]
	 *  \param set Referenz auf die Variable, die die Schl�ssel-Werte-Paare enth�lt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool Put(LogicalInstance* inst, const std::string& peer, XmlRpc::XmlRpcValue& set);
	//! Gibt den Parameter zu einer gegebenen ID zur�ck
	/*! \param id ID des gew�nschten Parameters
	 *  \return das gew�nschte Parameter-Objekt im Erfolgsfall, \c NULL im Fehlerfall
	 */
	HSSParameter* GetParameter(const std::string& id);
	//! Liest die Parametersetbeschreibung aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode& root_node);
	//! Liefert die aus der XML-Datei gelesene ID des Parametersets zur�ck
	/*! Auf diese ID beziehen sich die Easymode-Profile an der Oberfl�che.
	 */
    const std::string& GetId();
	//! Liefert den aus der XML-Datei gelesenen Typ des Parametersets zur�ck
	/*! Sollte immer \c MASTER, \c VALUES oder \c LINK zur�ckgeben.
	 */
    const std::string& GetType();
	//! Wird beim Erzeugen eines Ger�te- oder Kanalobjekts f�r alle zugeordneten Parametersets aufgerufen
	/*! Dieser Aufruf wird weitergereicht an HSSParameter::SetupInstance().
	 */
	bool SetupInstance(LogicalInstance* inst);
	//! Liefert zur�ck, ob es sich um ein Parameterset f�r eine Verkn�pfung handelt
	virtual bool IsLinkset()=0;
	//! Verarbeitet eine eingehende Nachricht, z.B. ein vom zugeh�rigen Ger�t gesendetes Funkpaket
	/*! Ist nur relevant, wenn es sich nicht um ein Parameterset f�r eine Verkn�pfung handelt
	 *  Ruft f�r alle HSSParameter, die sich f�r eingehende Nachrichten anhand der id der
	 *  FrameDescription (siehe FrameDescription::GetId()) registriert haben
	 *  (siehe SubscribeToEventFrame).
	 *  \param inst Zeiger auf das Kanalobjekt, dem die eingehende Nachricht zugeordnet ist
	 *  \param frame Referenz auf die zu verarbeitende Nachricht
	 *  \param fd Zeiger auf die zu \c msg passende abstrakte Beschreibung der Nachricht
	 */
	void ProcessIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, bool forwardedFrame = false);
	//! Registrierung eines Parameters f�r eine eingehende Nachricht
	/*! Wird w�hrend der Initialisierung einiger Objekte von von HSSPhysicalDataInterface abgeleiteten
	 *  Klassen aufgerufen. Diese registrieren sich dadurch f�r f�r sie interessante eingehende
	 *  Nachrichten.
	 *  \param param Parameter, der bei einer eingehenden Nachricht durch Aufruf von 
	 *               HSSParameter::ProcessIncomingFrame() informiert werden soll.
	 *  \param frame_id ID der FrameDescription, auf die sich die Registrierung bezieht
	 */
	void SubscribeToEventFrame(HSSParameter* param, const std::string& frame_id);
protected:
	//! ID des Parametersets f�r die XmlRpc-Schnittstelle
    std::string id;
	//! Typ des Parametersets f�r die XmlRpc-Schnittstelle
    std::string type;
	//! Typedef f�r die Verwaltung der Parameter
    typedef std::map<std::string, HSSParameter*> params_t;
	//! Map f�r die Verwaltung der Parameter
    params_t params;
	//! Typedef f�r die Verteilung eingehender Nachrichten an die Parameter
	typedef std::vector<HSSParameter*> t_vec_params;
	//! Noch ein Typedef f�r die Verteilung eingehender Nachrichten an die Parameter
	typedef std::map<std::string, t_vec_params> t_map_event_subscriptions;
	//! Map f�r die Verteilung eingehender Nachrichten an die Parameter
	t_map_event_subscriptions map_event_subscriptions;
	//! Typedef f�r unmittelbar nach dem Anlernen an das Ger�t zu �bertragene Parameterwerte
	typedef std::map<std::string, XmlRpc::XmlRpcValue> enforced_values_t;
	//! Map f�r unmittelbar nach dem Anlernen an das Ger�t zu �bertragene Parameterwerte
	/*! Diese Map wird in InitFromXml() aus der XML-Datei eingelesen. Das tats�chliche �bertragen
	 *  erfolgt in SetEnforcedValues() der abgeleiteten Klassen. Die �bertragung geh�rt nat�rlich
	 *  in HSSParamset. Die Zuordnung zu den abgeleiteten Klassen hat historische Gr�nde und sollte
	 *  ge�ndert werden.
	 */
	enforced_values_t enforced_values;
};

#endif // _HSSPARAMSET_H_

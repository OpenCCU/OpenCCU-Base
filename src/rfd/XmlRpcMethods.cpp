/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFManager.h"
#include "XmlRpcMethods.h"
#include "generated-rfd-version.h"
#include <Logger.h>

using namespace XmlRpc;

//#define LOG_METHOD_CALLS

#ifdef LOG_METHOD_CALLS

//! Hilfsklasse f�r das Loggen von XmlRpc-Aufrufen
/*! Das Loggen wird �ber das Makro \c LOG_METHOD_CALLS aktiviert.
 *  Das Makro sorgt daf�r, dass alle von XmlRpcServerMethod abgeleiteten Klassen von dieser Klasse abgeleitet
 *  werden und die Methoden execute() dieser Klassen dann do_execute hei�en.
 */
class XmlRpcDebugMethod : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcDebugMethod(const char* name, XmlRpcServer* s):XmlRpcServerMethod(name, s){};
	//! Loggen des Aufrufes und Aufruf von do_execute() der abgeleiteten Klasse
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		LOG(Logger::LOG_DEBUG, "XmlRpcMethod %s(%s)", _name.c_str(), params.toText().c_str());
		try{
			do_execute(params, result);
		}catch(XmlRpcException& e){
			LOG(Logger::LOG_DEBUG, "XmlRpcMethod %s(%s) exception %s",  _name.c_str(), params.toText().c_str(), e.getMessage().c_str());
			throw;
		}
		LOG(Logger::LOG_DEBUG, "XmlRpcMethod %s(%s)=%s", _name.c_str(), params.toText().c_str(), result.toText().c_str());
	}
	//! Pur virtuelle Funktion die in den abgeleiteten Klassen als execute() implementiert ist
	virtual void do_execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)=0;
};
#define XmlRpcServerMethod XmlRpcDebugMethod
#define execute(x,y) do_execute(x,y)
#endif

//! XmlRpc-Methode Array&lt;DeviceDescription&gt; listDevices()
/*!
 * Diese Methode gibt alle dem Schnittstellenproze� bekannten Ger�te in Form von
 * Ger�tebeschreibungen zur�ck. 
 *
 */
class XmlRpcMethodListDevices : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodListDevices(XmlRpcServer* s):XmlRpcServerMethod("listDevices", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->ListDevices(&result)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Array&lt;DeviceDescription&gt; listTeams()
/*!
 * Diese Methode gibt alle dem Schnittstellenproze� bekannten Teams (z.B. Rauchmeldergruppen) in Form von
 * Ger�tebeschreibungen zur�ck. 
 *
 */
class XmlRpcMethodListTeams : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodListTeams(XmlRpcServer* s):XmlRpcServerMethod("listTeams", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->ListTeams(&result)){
			throw XmlRpcException("Failure");
		}
	}
};


//! XmlRpc-Methode DeviceDescription getDeviceDescription(String address)
/*!
 * Diese Methode gibt die Ger�tebeschreibung des als "address" �bergebenen Ger�tes
 * zur�ck. 
 *
 */
class XmlRpcMethodGetDeviceDescription : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetDeviceDescription(XmlRpcServer* s):XmlRpcServerMethod("getDeviceDescription", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->GetDeviceDescription(params[0], &result)){
			throw XmlRpcException("Failure");
		}
	}
};


//! XmlRpc-Methode void deleteDevice(String address, Integer flags)
/*!
 * Diese Methode l�scht ein angelerntes Ger�t
 *
 * Parameter:
 * - \c address Adresse des zu l�schenden Ger�tes
 * - \c flags ist ein bitweises oder folgender Werte:
 *   - 0x01 (\c DELETE_FLAG_RESET) : Das Ger�t wird vor dem L�schen in den Werkszustand zur�ckgesetzt
 *   - 0x02 (\c DELETE_FLAG_FORCE) : Das Ger�t wird auch gel�scht, wenn es nicht erreichbar ist
 *   - 0x04 (\c DELETE_FLAG_DEFER) : Wenn das Ger�t nicht erreichbar ist, wird es bei n�chster Gelegenheit gel�scht
 */

class XmlRpcMethodDeleteDevice : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodDeleteDevice(XmlRpcServer* s):XmlRpcServerMethod("deleteDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		int flags=0;
		if(params[1].getType()==XmlRpcValue::TypeBoolean && (bool&)params[1])flags|=RFManager::DELETE_FLAG_RESET;
		if(params[2].getType()==XmlRpcValue::TypeBoolean && (bool&)params[2])flags|=RFManager::DELETE_FLAG_FORCE;
		if(params[1].getType()==XmlRpcValue::TypeInt)flags=params[1];
		if(!RFManager::GetSingleton()->DeleteDevice(params[0], flags)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void abortDeleteDevice(String address)
/*!
 * Bricht das L�schen eines Ger�ts ab.
 * Die Methode setzt das DELETE_DEFFERED Flag zur�ck.
 *
 * Parameter:
 * - \c address Adresse des betreffenden
 */
class XmlRpcMethodAbortDeleteDevice : public XmlRpcServerMethod
{
public:
	//! Konstruktor
	XmlRpcMethodAbortDeleteDevice(XmlRpcServer* s):XmlRpcServerMethod("abortDeleteDevice", s) {};

	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if (!RFManager::GetSingleton()->AbortDeleteDevice(params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode ParamsetDescription getParamsetDescription(String address, String paramset_type)
/*!
 * Mit dieser Methode wird die Beschreibung eines Parameter-Sets ermittelt. 
 *
 * Parameter:
 * - \c address ist die Adresse eines logischen Ger�tes (z.B. von listDevices zur�ckgegeben)
 * - \c paramset_type ist \c "MASTER", \c "VALUES" oder \c "LINK".
 *
 */
class XmlRpcMethodGetParamsetDescription : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetParamsetDescription(XmlRpcServer* s):XmlRpcServerMethod("getParamsetDescription", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		result.assertStruct();
		if(!RFManager::GetSingleton()->GetParamsetDescription((std::string&)params[0], (std::string&)params[1], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void putParamset(String address, String paramset_key, Paramset set)
/*!
 * Mit dieser Methode wird ein komplettes Parameter-Set f�r ein logisches Ger�t geschrieben.
 *
 * Parameter:
 * - \c address ist die Addresses eines logischen Ger�tes
 * - \c paramset_key ist \c "MASTER", \c "VALUES" oder die Adresse eines Kommunikationspartners
 *   f�r das entsprechende Link-Parameter-Set (siehe getLinkPeers).
 * - \c set ist das zu schreibende Parameter-Set. In \c set nicht vorhandene Member
 *   werden einfach nicht geschrieben und behalten ihren alten Wert.
 *
 */
class XmlRpcMethodPutParamset : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodPutParamset(XmlRpcServer* s):XmlRpcServerMethod("putParamset", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() == 4) {
			if(!RFManager::GetSingleton()->PutParamsetValues((std::string&)params[0], (std::string&)params[1], params[2], (std::string&)params[3])){
				throw XmlRpcException("Failure");
			}
		}
		else {
			if(!RFManager::GetSingleton()->PutParamsetValues((std::string&)params[0], (std::string&)params[1], params[2], std::string(""))){
				throw XmlRpcException("Failure");
			}
		}
	}
};

//! XmlRpc-Methode Paramset getParamset(String address, String paramset_key)
/*!
 * Mit dieser Methode wird ein komplettes Parameter-Set f�r ein logisches Ger�t gelesen
 *
 * Parameter:
 * - \c address ist die Addresses eines logischen Ger�tes. 
 * - \c paramset_key ist \c "MASTER", \c "VALUES" oder die Adresse eines Kommunikationspartners
 *   f�r das entsprechende Link-Parameter-Set (siehe getLinkPeers). 
 *
 */
class XmlRpcMethodGetParamset : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetParamset(XmlRpcServer* s):XmlRpcServerMethod("getParamset", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		result.assertStruct();
		if(params.size()==3)
		{
			if(!RFManager::GetSingleton()->GetParamsetValues((std::string&)params[0], (std::string&)params[1],params[2], &result)){
						throw XmlRpcException("Failure");
					}
		}
		else
		{

		if(!RFManager::GetSingleton()->GetParamsetValues((std::string&)params[0], (std::string&)params[1],0, &result)){
			throw XmlRpcException("Failure");
		}
		}

	}
};

//! XmlRpc-Methode void determineParameter(String address, String paramset, String parameter)
/*!
 * Mit dieser Methode wird das automatische Ermitteln eines Parameterwertes initiiert.
 *
 * Parameter:
 * - \c address ist die Addresses eines logischen Ger�tes. 
 * - \c paramset ist \c "MASTER", \c "VALUES" oder die Adresse eines Kommunikationspartners
 * - \c parameter ist die Id des zu ermittelnden Parameters
 */
class XmlRpcMethodDetermineParameter : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodDetermineParameter(XmlRpcServer* s):XmlRpcServerMethod("determineParameter", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->DetermineParameter((std::string&)params[0], (std::string&)params[1], (std::string&)params[2])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode ValueType getValue(String address, String value_key)
/*!
 * Mit dieser Methode wird ein einzelner Wert aus dem Parameter-Set \c "VALUES" gelesen.
 *
 * Parameter:
 * - \c address ist die Addresse eines logischen Ger�tes
 * - \c value_key ist der Name des zu lesenden Wertes. Die m�glichen Werte f�r value_key ergeben sich aus der
 * ParamsetDescription des entsprechenden Parameter-Sets \c "VALUES". 
 *
 */
class XmlRpcMethodGetValue : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetValue(XmlRpcServer* s):XmlRpcServerMethod("getValue", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() == 3)
		{
			if(!RFManager::GetSingleton()->GetValue(params[0], params[1],params[2], &result)){
						throw XmlRpcException("Failure");
			}
		}
		else
		{
		if(!RFManager::GetSingleton()->GetValue(params[0], params[1],0, &result)){
			throw XmlRpcException("Failure");
		}
		}
	}
};

//! XmlRpc-Methode void setValue(String address, String value_key)
/*! 
 * Mit dieser Methode wird ein einzelner Wert aus dem Parameter-Set "VALUES" geschrieben.
 *
 * Parameter:
 * - \c address ist die Addresse eines logischen Ger�tes
 * - \c value_key ist der Name des zu schreibenden Wertes. Die m�glichen Werte f�r value_key ergeben sich aus der
 * ParamsetDescription des entsprechenden Parameter-Sets "VALUES".
 * - \c rxmode RX mode for this call. Possible values are "WAKEUP" and "BURST". (Optional parameter; Type is String.)
 *
 */
class XmlRpcMethodSetValue : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetValue(XmlRpcServer* s):XmlRpcServerMethod("setValue", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() == 4) {//DTAG Feature: dynamic rx_mode change
			if(!RFManager::GetSingleton()->SetValue(params[0], params[1], params[2], (std::string&)params[3])){
				throw XmlRpcException("Failure");
			}
		}
		else {
			if(!RFManager::GetSingleton()->SetValue(params[0], params[1], params[2], std::string(""))){
				throw XmlRpcException("Failure");
			}
		}
	}
};


//! XmlRpc-Methode Array&lt;String&gt;getLinkPeers(String address)
/*! 
 * Diese Methode gibt alle einem logischen Ger�t zugeordneten Kommunikationspartner zur�ck. Die
 * zur�ckgegebenen Werte k�nnen als Parameter "paramset_key" f�r getParamset und
 * putParamset verwendet werden.
 *
 * Parameter:
 * - \c address ist die Addresses eines logischen Ger�tes. 
 */
class XmlRpcMethodGetLinkPeers : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetLinkPeers(XmlRpcServer* s):XmlRpcServerMethod("getLinkPeers", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		std::vector<std::string> peers;
		if(!RFManager::GetSingleton()->GetLinkPeers((std::string&)params[0], &peers)){
			throw XmlRpcException("Failure");
		}
		result.assertArray(peers.size());
		for(unsigned int i=0;i<peers.size();i++){
			result[i]=peers[i];
		}
	}
};

//! XmlRpc-Methode Array&lt;Struct&gt;getLinks(String address, Integer flags)
/*! 
 * Diese Methode gibt alle einem logischen Kanal oder Ger�t zugeordneten Kommunikationsbeziehungen zur�ck.
 *
 * Parameter:
 * - \c address ist die Kanal- oder Ger�teadresse des logischen Objektes, auf das sich die Abfrage bezieht.
 *   Bei \c address=="" werden alle Kommunikationsbeziehungen des Schnittstellenprozesses zur�ckgegeben
 *
 * - \c flags ist ein bitweises "oder" folgender Werte:
 *   - 0x01 (GL_FLAG_GROUP) : wenn address einen Kanal bezeichnet, der sich in einer Gruppe befindet, werden die
 *     Kommunikationsbeziehungen f�r alle Kan�le der Gruppe zur�ckgegeben
 *   - 0x02 (GL_FLAG_SENDER_PARAMSET) : Das Feld SENDER_PARAMSET des R�ckgabewertes wird gef�llt
 *   - 0x04 (GL_FLAG_RECEIVER_PARAMSET) : Das Feld RECEIVER_PARAMSET des R�ckgabewertes wird gef�llt
 *   - 0x08 (GL_FLAG_SENDER_DESCRIPTION) : Das Feld SENDER_DESCRIPTION des R�ckgabewertes wird gef�llt
 *   - 0x10 (GL_FLAG_RECEIVER_DESCRIPTION) : Das Feld RECEIVER_DESCRIPTION des R�ckgabewertes wird gef�llt
 *   flags ist optional. Defaultwert ist 0x00.
 *
 * R�ckgabewert:
 * Der R�ckgabewert ist ein Array von Strukturen. Jede dieser Strukturen enth�lt die folgenden Felder:
 * - \c String \c SENDER : Adresse des Senders der Kommunikationsbeziehung
 * - \c String \c RECEIVER : Adresse des Empf�ngers der Kommunikationsbeziehung
 * - \c String \c NAME : Name der Kommunikationsbeziehung
 * - \c String \c DESCRIPTION : Beschreibung der Kommunikationsbeziehung
 * - \c Paramset \c SENDER_PARAMSET : Parametersatz dieser Kommunikationsbeziehung f�r die Senderseite
 * - \c Paramset \c RECEIVER_PARAMSET : Parametersatz dieser Kommunikationsbeziehung f�r die Empf�ngerseite
 * - \c DeviceDescription \c SENDER_DESCRIPTION : Ger�tebeschreibung des Senders
 * - \c DeviceDescription \c RECEIVER_DESCRIPTION : Ger�tebeschreibung des Empf�ngers
 */

class XmlRpcMethodGetLinks : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetLinks(XmlRpcServer* s):XmlRpcServerMethod("getLinks", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		result.assertArray(0);
		params[1];
		if(!RFManager::GetSingleton()->GetLinks((std::string&)params[0], (int&)params[1], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void addLink(String sender_address, String receiver_address, String name, String description)
/*! 
 * Diese Methode erstellt eine Kommunikationsbeziehung zwischen zwei logischen Ger�ten. Die
 * Parameter "sender_address" und "receiver_address" bezeichnen die beiden zu verkn�pfenden Partner.
 * Die Parameter "name" und "description" sind optional und beschreiben die Verkn�pfung n�her.
 *
 */
class XmlRpcMethodAddLink : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodAddLink(XmlRpcServer* s):XmlRpcServerMethod("addLink", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[1];
		if(!RFManager::GetSingleton()->AddLink((std::string&)params[0], (std::string&)params[1])){
			throw XmlRpcException("Failure");
		}
		if(params.size()>=3){
			params[3];
			if(!RFManager::GetSingleton()->SetLinkInfo((std::string&)params[0], (std::string&)params[1], (std::string&)params[2], (std::string&)params[3])){
				throw XmlRpcException("Failure: link successfully added, but SetLinkInfo failed");
			}
		}
	}
};

//! XmlRpc-Methode void setLinkInfo(String sender_address, String receiver_address, String name, String description)
/*! 
 * Diese Methode setzt den Namen und die Beschreibung f�r eine Kommunikationsbeziehung zwischen zwei logischen 
 * Ger�ten. Die Parameter "sender_address" und "receiver_address" bezeichnen die beiden zu verkn�pfenden Partner.
 * Die Parameter "name" und "description" beschreiben die Verkn�pfung n�her.
 *
 */
class XmlRpcMethodSetLinkInfo : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetLinkInfo(XmlRpcServer* s):XmlRpcServerMethod("setLinkInfo", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[3];
		if(!RFManager::GetSingleton()->SetLinkInfo((std::string&)params[0], (std::string&)params[1], (std::string&)params[2], (std::string&)params[3])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Array&lt;String&gt; getLinkInfo(String sender_address, String receiver_address)
/*! 
 * Diese Methode gibt den Namen und die Beschreibung f�r eine Kommunikationsbeziehung zur�ck. Die
 * Parameter "sender_address" und "receiver_address" bezeichnen die beiden verkn�pften Partner.
 *
 * Der R�ckgabewert ist ein Array bestehend aus zwei Strings. Der erste ist der Name und der zweite die
 * Beschreibung.
 */
class XmlRpcMethodGetLinkInfo : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetLinkInfo(XmlRpcServer* s):XmlRpcServerMethod("getLinkInfo", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[1];
		result["NAME"];
		result["DESCRIPTION"];
		if(!RFManager::GetSingleton()->GetLinkInfo((std::string&)params[0], (std::string&)params[1], &((std::string&)result["NAME"]), &((std::string&)result["DESCRIPTION"]))){
			throw XmlRpcException("Failure");
		}

	}
};

//! XmlRpc-Methode void removeLink(String sender_address, String receiver_address)
/*! 
 * Diese Methode l�scht eine Kommunikationsbeziehung zwischen zwei Ger�ten. Die
 * Parameter "sender_address" und "receiver_address" bezeichnen die beiden verkn�pften Partner
 * deren Kommunikationsbeziehung gel�scht werden soll.
 *
 */
class XmlRpcMethodRemoveLink : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodRemoveLink(XmlRpcServer* s):XmlRpcServerMethod("removeLink", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->RemoveLink((std::string&)params[0], (std::string&)params[1])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void clearConfigCache(String address)
/*! 
 * Diese Methode l�scht alle zu einem Ger�t in der Zentrale gespeicherten Konfigurationsdaten.
 * Nur zu Debugzwecken zu verwenden.
 *
 */
class XmlRpcMethodClearConfigCache : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodClearConfigCache(XmlRpcServer* s):XmlRpcServerMethod("clearConfigCache", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->ClearConfigCache((std::string&)params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void restoreConfigToDevice(String address)
/*! 
 * Diese Methode �bertr�gt alle zu einem Ger�t in der Zentrale gespeicherten Konfigurationsdaten 
 * erneut an das Ger�t
 *
 */
class XmlRpcMethodRestoreConfigToDevice : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodRestoreConfigToDevice(XmlRpcServer* s):XmlRpcServerMethod("restoreConfigToDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->RestoreConfigToDevice((std::string&)params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void activateLinkParamset(String address, String peer_address, Boolean long_press)
/*! 
 * Mit dieser Methode wird ein Link-Parameterset aktiviert. Das logische Ger�t verh�lt sich dann so
 * als ob es direkt von dem entsprechenden zugeordneten Ger�t angesteuert worden w�re. Hiermit
 * kann z.B. ein Link-Parameterset getestet werden.
 *
 * Parameter:
 * - \c address ist die Addresses des anzusprechenden Ger�tes
 * - \c peer_address ist die Addresse des Kommunikationspartners, dessen Link-Parameter-Set aktiviert werden soll.
 * - \c long_press gibt an, ob das Parameterset f�r den langen Tastendruck aktiviert werden soll.
 */
class XmlRpcMethodActivateLinkParamset : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodActivateLinkParamset(XmlRpcServer* s):XmlRpcServerMethod("activateLinkParamset", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.getType() == XmlRpcValue::TypeArray && params.size() >= 3
				&& params[0].valid() && params[1].valid() && params[2].valid() ) {
			if(!RFManager::GetSingleton()->ActivateLinkParamset((std::string&)params[0], (std::string&)params[1], (bool&)params[2])){
				throw XmlRpcException("Failure");
			}
		}
		else {
			throw XmlRpcException("Insufficient parameters.", -1);
		}
	}
};

//! XmlRpc-Methode init(String url, String interface_id)
/*! 
 * Mit dieser Methode registriert sich ein Logikprozess beim Schnittstellenproze�.
 * Der Schnittstellenproze� wird daraufhin die Liste der angelerneten Ger�te mit dem Logikproze�
 * abgleichen. Dazu ruft er listDevices(), deleteDevice() und newDevice() auf.
 * Der Parameter "url" gibt die Adresse des XmlRpc-Servers an, unter der der Logikproze� zu erreichen ist.
 * Der Parameter "interface_id" teilt dem Schnittstellenproze� die Id mit unter der er sich
 * gegen�ber dem Logikproze� identifiziert.
 * Bei \c interface_id=="" wird der Logikproze� vom Interfaceproze� abgemeldet.
 */
class XmlRpcMethodInit : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodInit(XmlRpcServer* s):XmlRpcServerMethod("init", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[1];
		RFManager::GetSingleton()->PlatformInit(params[0], params[1]);
	}
};

//! XmlRpc-Methode void setInstallMode(Boolean on, Integer seconds)
/*! 
 * Diese Methode aktiviert und deaktiviert den Installations-Modus, in dem neue Ger�te am BidCoS-RF Schnittstellenproze�
 * angemeldet werden k�nnen.
 * Der Parameter �on� bestimmt, ob der Installations-Modus aktiviert oder deaktiviert werden soll.
 * Der Parameter "seconds" ist optional und gibt die Zeit in Sekunden an, die der Install-Mode aktiv bleiben soll.
 * "seconds" darf maximal 600 sein. Defaultwert ist 60.
 *
 */
class XmlRpcMethodSetInstallMode : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetInstallMode(XmlRpcServer* s):XmlRpcServerMethod("setInstallMode", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		int time=0;
		InstallModes mode = INSTALL_NORMAL;
		if((bool)params[0]){
			if(params.size()>=2)time=params[1];
			else time=60;
			if(params.size()>=3)
			{
				if(params[2].getType() == XmlRpcValue::TypeInt) {
					if(((int)params[2] >= (int)INSTALL_OFF) && ((int)params[2] <= (int)INSTALL_PUSH_DEFAULT_CONFIG))
					{
						mode = (InstallModes)(int)params[2];
					}
					else {
						throw XmlRpcException("Wrong usage of setInstallMode (invalid value for parameter mode)", -1);
					}
				}
				else if(params[2].getType() == XmlRpcValue::TypeString) {
					RFManager::GetSingleton()->SetInstallMode(INSTALL_DEVICE_WHITELIST, time, params[2]);
					return;
				}
				else {
					throw XmlRpcException("Wrong usage of setInstallMode", -1);
				}
			}
		}
		else
		{
			mode = INSTALL_OFF;
		}
		RFManager::GetSingleton()->SetInstallMode(mode,time);
	}
};

//! XmlRpc-Methode Integer getInstallMode()
/*! 
 * Diese Methode gibt die verbleibende Restzeit im Installations-Modus in Sekunden zur�ck.
 * 0 bedeutet, der Installations-Modus ist nicht aktiv.
 *
 */
class XmlRpcMethodGetInstallMode : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetInstallMode(XmlRpcServer* s):XmlRpcServerMethod("getInstallMode", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		(int&)result=RFManager::GetSingleton()->GetInstallMode();
	}
};

//! XmlRpc-Methode String getKeyMismatchDevice(bool reset)
/*!
 * Diese Methode gibt die Seriennummer des letzten Ger�tes zur�ck, das aufgrund eines falschen AES-Schl�ssels
 * nicht angelernt werden konnte.
 * 
 * Parameter:
 * - \c reset Gibt an, ob die zur�ckgegebene Information zur�ckgesetzt werden soll.
 *
 */
class XmlRpcMethodGetKeyMismatchDevice : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetKeyMismatchDevice(XmlRpcServer* s):XmlRpcServerMethod("getKeyMismatchDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		(std::string&)result=RFManager::GetSingleton()->GetKeyMismatchDevice(params[0]);
	}
};

//! XmlRpc-Methode String getParamsetId(String address, String type)
/*!
 * Diese Methode gibt die Id eines Parametersets zur�ck
 * Die Id eines Parametersets wird verwendet f�r die Zuordnung von Easymode-Seiten zum Parameterset.
 * Die Id wird aus der Ger�tebeschreibungsdatei gelesen und �ber getParamsetId() an die Oberfl�che
 * durchgereicht.
 *
 * Parameter:
 * - \c address ist die Addresses des anzusprechenden Ger�tes
 * - \c type Typ des Parametersets (\c "MASTER", \c "VALUES" oder \c "LINK")
 */
class XmlRpcMethodGetParamsetId : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetParamsetId(XmlRpcServer* s):XmlRpcServerMethod("getParamsetId", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		std::string& id=result;
		if(!RFManager::GetSingleton()->GetParamsetId(params[0], params[1], &id)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Boolean reportValueUsage(String address, String value_id, Integer ref_counter)
/*!
 * Diese Methode teilt dem Interfaceproze� mit, wie oft ein Wert innerhalb der Logikschicht
 * verwendet wird. Dadurch kann der Interfaceproze� die Verbindung mit der entsprechenden Komponente
 * herstellen bzw. l�schen. Diese Funktion sollte bei jeder �nderung aufgerufen werden.
 * 
 * Der R�ckgabewert ist true, wenn die Aktion sofort durchgef�hrt wurde. Er ist false, wenn die entsprechende
 * Komponente nicht erreicht werden konnte und vom Benutzer zun�chst in den Config-Mode gebracht werden mu�.
 * Der Interfaceproze� hat dann aber die neue Einstellung �bernommen und wird sie bei n�chster Gelegenheit
 * automatisch an die Komponente �bertragen.
 * In diesem Fall ist dann auch der Wert "CONFIG_PENDING" im Kanal "MAINTENANCE" der Komponente
 * gesetzt.
 *
 */
class XmlRpcMethodReportValueUsage : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodReportValueUsage(XmlRpcServer* s):XmlRpcServerMethod("reportValueUsage", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[2];
		(bool&)result=RFManager::GetSingleton()->ReportValueUsage(params[0], params[1], params[2]);
	}
};

//! XmlRpc-Methode DeviceDescription addDevice(String serial_number)
/*!
 * Diese Methode lernt ein Ger�t anhand der Seriennummer an die Zentrale an.
 * Diese Funktion wird nicht von jedem Ger�t unterst�tzt.
 * R�ckgabewert ist die DeviceDescription des neu angelernten Ger�tes.
 *
 */
class XmlRpcMethodAddDevice : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodAddDevice(XmlRpcServer* s):XmlRpcServerMethod("addDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() >= 2)
		{
			if(!RFManager::GetSingleton()->AddDevice((InstallModes)((int)params[1]),params[0], &result)){
			throw XmlRpcException("Failure");
		}
		}
		else
		{
			if(!RFManager::GetSingleton()->AddDevice(INSTALL_NORMAL,params[0], &result)){
				throw XmlRpcException("Failure");
			}
		}
	}
};

//! XmlRpc-Methode void changeKey(String passphrase)
/*!
 * Diese Methode �ndert den von der Zentrale verwendeten AES-Schl�ssel. Der Schl�ssel
 * wird ebenfalls in allen angelernten Ger�ten getauscht.
 *
 */
class XmlRpcMethodChangeKey : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodChangeKey(XmlRpcServer* s):XmlRpcServerMethod("changeKey", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->ChangeAESKey(params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void setTempKey(String passphrase)
/*!
 * Diese Methode �ndert den von der Zentrale verwendeten tempor�ren AES-Schl�ssel.
 * Der tempor�re Schl�ssel wird f�r das Anlernen von Ger�ten ben�tigt, die
 * einen der Zentrale nicht bekannten Schl�ssel gespeichert haben.
 */
class XmlRpcMethodSetTempKey : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetTempKey(XmlRpcServer* s):XmlRpcServerMethod("setTempKey", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->SetTempAESKey(params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Integer logLevel([optional]int level)
/*!
 * Diese Methode gibt den aktuellen Log-Level zur�ck bzw. setzt diesen wenn der optionale Parameter level
 * �bergeben wird.
 *
 */
class XmlRpcMethodLogLevel : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodLogLevel(XmlRpcServer* s):XmlRpcServerMethod("logLevel", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if ((params.getType() == XmlRpc::XmlRpcValue::TypeArray) && (params.size()>=1)) {
			logger->SetLevel((Logger::LogLevel)((int&)params[0]));
		}
		result=logger->GetLevel();
	}
};

//! XmlRpc-Methode void setTeam(String channel_address, String team_address)
/*!
 * void setTeam(String channel_address, String team_address)
 *
 * Diese Methode f�gt den Kanal channel_address zum Team team_address hinzu. 
 * Bei team_address=="" wird der Kanal channel_address seinem eigenen Team zugeordnet
 */
class XmlRpcMethodSetTeam : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetTeam(XmlRpcServer* s):XmlRpcServerMethod("setTeam", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[1];
		if(!RFManager::GetSingleton()->SetTeam((std::string&)params[0], (std::string&)params[1])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Struct rssiInfo()
/*!
 * Gibt ein zweidimensionales assoziatives Array zur�ck, dessen Schl�ssel die Ger�teseriennummern sind.
 * Die Felder des assoziativen Arrays sind Tupel, die die Empfangsfeldst�rken zwischen beiden 
 * Schl�sselger�ten f�r beide Richtungen in dbm angeben. ein Wert von 65536 bedeutet, dass keine Informationen
 * vorliegen.
 *
 */
class XmlRpcMethodRSSIInfo : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodRSSIInfo(XmlRpcServer* s):XmlRpcServerMethod("rssiInfo", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RFManager::GetSingleton()->GetRSSIInfo(&result);
	}
};

//! XmlRpc-Methode void setBidcosInterface(String device_address, String interface_address, bool roaming)
/*!
 * void setBidcosInterface(String device_address, String interface_address, bool roaming)
 *
 * Diese Methode setzt das BidCoS-Interface �ber das ein Ger�t angesprochen werden soll.
 *
 * Parameter:
 * - \c device_address ist die Addresses des Ger�tes
 * - \c interface_address ist die Adresse (Seriennummer) des BidCoS-Interfaces, dem das Ger�t zugeordnet werden soll.
 * - \c roaming gibt an, ob die Zuordnung zwischen Ger�t und Interface abh�ngig von der Empfangsfeldst�rke
 *              dynamisch ge�ndert werden darf.
 */
class XmlRpcMethodSetBidcosInterface : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetBidcosInterface(XmlRpcServer* s):XmlRpcServerMethod("setBidcosInterface", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[2];
		if(!RFManager::GetSingleton()->SetBidcosInterface((std::string&)params[0], (std::string&)params[1], (bool&)params[2])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Array&lt;Struct&gt; listBidcosInterfaces()
/*!
 * Array&lt;Struct&gt; listBidcosInterfaces()
 *
 * Diese Methode gibt eine Liste aller vorhandenen BidCoS-Interfaces zur�ck.
 *
 * Felder des R�ckgabewertes:
 * - \c String \c ADDRESS ist die Adresse (Seriennummer) des BidCoS-Interfaces
 * - \c String \c DESCRIPTION ist eine textuelle Beschreibung des Interfaces
 * - \c bool \c CONNECTED gibt an, ob derzeit eine Verbindung zum Transceiver besteht
 * - \c bool \c DEFAULT gibt an, ob es sich um das Default-Interface handelt
 */
class XmlRpcMethodListBidcosInterfaces : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodListBidcosInterfaces(XmlRpcServer* s):XmlRpcServerMethod("listBidcosInterfaces", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[2];
		if(!RFManager::GetSingleton()->ListBidcosInterfaces(&result)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Array&lt;Array&gt; getServiceMessages()
/*!
 * Array&lt;Array&gt; getServiceMessages()
 *
 * Diese Methode gibt eine Liste aller vorhandenen Servicemeldungen zur�ck.
 *
 * Der R�ckgabewert ist ein Array mit einem Feld pro Servicemeldung. Jedes Feld ist wiederum ein Array mit 
 * drei Feldern:
 * - \c String \c Adresse (Seriennummer) des Kanals, der die Servicemeldung generiert hat
 * - \c String \c ID der Servicemeldung (CONFIG_PENDING, UNREACH, etc.)
 * - \c variabel \c Wert der Servicemeldung
 */
class XmlRpcMethodGetServiceMessages : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetServiceMessages(XmlRpcServer* s):XmlRpcServerMethod("getServiceMessages", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RFManager::GetSingleton()->GetServiceMessages(&result);
	}
};

//! XmlRpc-Methode Variant getMetadata(String object_id, String data_id)
/*!
 * Variant getMetadata(String object_id, String data_id)
 *
 * Diese Methode gibt ein Metadatum zu einem Objekt zur�ck.
 *
 * Parameter:
 * - \c object_id ist die Id des Metadaten-Objekts. �blicherweise ist dies die Adresse eines Ger�tes oder Kanals.
 *              Es k�nnen aber auch  eigene Metadaten-Objekte angelegt werden.
 * - \c data_id ist die Id des abzufragenden Metadatums
 */
class XmlRpcMethodGetMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetMetadata(XmlRpcServer* s):XmlRpcServerMethod("getMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
        if(!RFManager::GetSingleton()->MetadataGet(((std::string&)params[0]).c_str(), ((std::string&)params[1]).c_str(), &result)){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode void setMetadata(String object_id, String data_id, Variant value)
/*!
 * void setMetadata(String object_id, String data_id, Variant value)
 *
 * Diese Methode setzt ein Metadatum zu einem Objekt.
 *
 * Parameter:
 * - \c object_id ist die Id des Metadaten-Objekts. �blicherweise ist dies die Adresse eines Ger�tes oder Kanals.
 *              Es k�nnen aber auch eigene Metadaten-Objekte angelegt werden.
 * - \c data_id ist die Id des zu setzenden Metadatums
 * - \c value ist der zu setzende Wert
 */
class XmlRpcMethodSetMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetMetadata(XmlRpcServer* s):XmlRpcServerMethod("setMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->MetadataSet(((std::string&)params[0]).c_str(), ((std::string&)params[1]).c_str(), params[2])){
			throw XmlRpcException("Failure");
		}
	}
};

//! XmlRpc-Methode Struct getAllMetadata(String object_id)
/*!
 * Struct getAllMetadata(String object_id)
 *
 * Diese Methode gibt alle Metadaten zu einem Objekt zur�ck.
 *
 * Parameter:
 * - \c object_id ist die Id des Metadaten-Objekts. �blicherweise ist dies die Adresse eines Ger�tes oder Kanals.
 *              Es k�nnen aber auch  eigene Metadaten-Objekte angelegt werden.
 */
class XmlRpcMethodGetAllMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetAllMetadata(XmlRpcServer* s):XmlRpcServerMethod("getAllMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!RFManager::GetSingleton()->MetadataGetAll(((std::string&)params[0]).c_str(), &result)){
			throw XmlRpcException("Failure");
		}
	}
};

class XmlRpcMethodGetVolatileMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetVolatileMetadata(XmlRpcServer* s):XmlRpcServerMethod("getVolatileMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		const char* dataId = ((std::string&)params[0]).c_str();
		
		if(!RFManager::GetSingleton()->MetadataGetVolatile(dataId, &result)){
			throw XmlRpcException("Failure");
		}
	}
};

class XmlRpcMethodHasVolatileMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodHasVolatileMetadata(XmlRpcServer* s):XmlRpcServerMethod("hasVolatileMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		const char* dataId = ((std::string&)params[0]).c_str();
		XmlRpc::XmlRpcValue value;
		
		result = XmlRpcValue(RFManager::GetSingleton()->MetadataGetVolatile(dataId, &value));
	}
};

class XmlRpcMethodSetVolatileMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodSetVolatileMetadata(XmlRpcServer* s):XmlRpcServerMethod("setVolatileMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		const char* dataId = ((std::string&)params[0]).c_str();
		XmlRpc::XmlRpcValue& value = params[1];
		
		if(!RFManager::GetSingleton()->MetadataSetVolatile(dataId, value)){
			throw XmlRpcException("Failure");
		}
	}
};

class XmlRpcMethodDeleteVolatileMetadata : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodDeleteVolatileMetadata(XmlRpcServer* s):XmlRpcServerMethod("deleteVolatileMetadata", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		const char* dataId = ((std::string&)params[0]).c_str();
		
		if(!RFManager::GetSingleton()->MetadataDeleteVolatile(dataId)){
			throw XmlRpcException("Failure");
		}
	}
};

class XmlRpcMethodGetVersion : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodGetVersion(XmlRpcServer* s):XmlRpcServerMethod("getVersion", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		result = RFD_VERSION;
	}
};

class XmlRpcMethodSetInterfaceClock : public XmlRpcServerMethod
{
public:
	//! Konstruktor
	XmlRpcMethodSetInterfaceClock(XmlRpcServer* s):XmlRpcServerMethod("setInterfaceClock", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		result = RFManager::GetSingleton()->SetInterfaceClock( (int)params[0], (int)params[1] );
	}
};

/*
 * Array<bool> updateFirmware(Array<String> devices)
 *
 * Diese Methode f�hrt ein Firmware-Update der in devices enthaltenen Ger�te durch.
 * Der R�ckgabewert gibt f�r jedes Ger�t an, ob das Firmware-Update erfolgreich war.
 *
 */
class XmlRpcMethodUpdateFirmware : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodUpdateFirmware(XmlRpcServer* s):XmlRpcServerMethod("updateFirmware", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size()>0){
					(bool&)result=RFManager::GetSingleton()->UpdateFirmware(params[0]);
				}
		/*for(int i=0;i<params.size();i++){
			(bool&)result[i]=RFManager::GetSingleton()->UpdateFirmware(params[i]);
		}*/
	}
};
class XmlRpcMethodReplaceDevice : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodReplaceDevice(XmlRpcServer* s):XmlRpcServerMethod("replaceDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() != 2)
		{
			throw XmlRpcException("Failure"); 
		}
		(bool&)result = RFManager::GetSingleton()->ReplaceDevice(params[0],params[1]);
		
	}
};
class XmlRpcMethodAddVirtualDeviceInstance : public XmlRpcServerMethod
{
public:
	XmlRpcMethodAddVirtualDeviceInstance(XmlRpcServer* s):XmlRpcServerMethod("addVirtualDeviceInstance", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() != 1)
		{
			throw XmlRpcException("Failure");
		}
		std::vector<unsigned char> sysinfo;
		for(int i = 0;i<params[0].size();++i)
		{
			sysinfo.push_back((unsigned char)(int)params[0][i]);
		}
		(bool&)result = RFManager::GetSingleton()->AddVirtualDeviceInstance(sysinfo);

	}
};
class XmlRpcMethodListReplaceableDevices : public XmlRpcServerMethod
{
public:
	XmlRpcMethodListReplaceableDevices(XmlRpcServer* s):XmlRpcServerMethod("listReplaceableDevices", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		bool retVal = false;
		if (params.size() == 1) {
			retVal = RFManager::GetSingleton()->ListReplaceableDevices(
					params[0], &result);
		} else if (params.size() == 2) {
			if ((params[1].getType() == XmlRpcValue::TypeInt)) {
				retVal = RFManager::GetSingleton()->ListReplaceableDevices(params[0], &result,(RFManager::DeviceReplaceLevel_t) (int)params[1]);
			}
		}
		if (!retVal) {
			throw XmlRpcException("Failure");
		}

	}
};

/** \brief Sets info led of HM2LGW.
 * Parameter:
 * Blink state:
 * 0 LED Off
 * 1 LED On
 * 2 LED Blink Slow
 * 3 LED Blink Fast
 */

class XmlRpcMethodSetRFLGWInfoLED : public XmlRpcServerMethod
{
public:
	//! Konstruktor
	XmlRpcMethodSetRFLGWInfoLED(XmlRpcServer* s):XmlRpcServerMethod("setRFLGWInfoLED", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		bool retVal = false;
		if (params.size() == 1) {
			retVal = RFManager::GetSingleton()->SetRFLGWInfoLED((unsigned int)(int)params[0]);
		} else
		if (!retVal) {
			throw XmlRpcException("Failure");
		}
	}
};

/** \brief Refresh list of user deployed device firmware files.
 */
class XmlRpcMethodRefreshDeployedDeviceFirmwareList : public XmlRpcServerMethod
{
public:
	//! Konstruktor
	XmlRpcMethodRefreshDeployedDeviceFirmwareList(XmlRpcServer* s):XmlRpcServerMethod("refreshDeployedDeviceFirmwareList", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RFManager::GetSingleton()->RefreshDeployedDeviceFirmwareList();
	}
};

/** \brief Ping Pong Feature. Calling this method generates a 'pong' event.
 * \details event(String centralSerial, "pong", String callerId
 * Params: String callerId
 * Return: boolean True
 */
class XmlRpcMethodPing : public XmlRpcServerMethod
{
public:
	//! Konstruktor
	XmlRpcMethodPing(XmlRpcServer* s):XmlRpcServerMethod("ping", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(params.size() != 1) {
			throw XmlRpcException("Failure");
		}
		else {
			if(params[0].getType() != XmlRpcValue::TypeString) {
				throw XmlRpcException("Parameter callerId must be of type string");
			}
			RFManager::GetSingleton()->Ping(params[0]);
			(bool&)result[0] = true;
		}
	}
};

void InitXmlRpcMethods(XmlRpcServer& s)
{
	static XmlRpcMethodGetParamsetDescription xmlRpcMethodGetParamsetDescription(&s);
	static XmlRpcMethodPutParamset xmlRpcMethodPutParamset(&s);
	static XmlRpcMethodGetParamset xmlRpcMethodGetParamset(&s);
	static XmlRpcMethodGetValue xmlRpcMethodGetValue(&s);
	static XmlRpcMethodSetValue xmlRpcMethodSetValue(&s);
	static XmlRpcMethodListDevices xmlRpcMethodListDevices(&s);
	static XmlRpcMethodListTeams xmlRpcMethodListTeams(&s);
	static XmlRpcMethodGetDeviceDescription xmlRpcMethodGetDeviceDescription(&s);
	static XmlRpcMethodGetLinkPeers xmlRpcMethodGetLinkPeers(&s);
	static XmlRpcMethodGetLinks xmlRpcMethodGetLinks(&s);
	static XmlRpcMethodAddLink xmlRpcMethodAddLink(&s);
	static XmlRpcMethodSetLinkInfo xmlRpcMethodSetLinkInfo(&s);
	static XmlRpcMethodGetLinkInfo xmlRpcMethodGetLinkInfo(&s);
	static XmlRpcMethodRemoveLink xmlRpcMethodRemoveLink(&s);
	static XmlRpcMethodClearConfigCache xmlRpcMethodClearConfigCache(&s);
	static XmlRpcMethodActivateLinkParamset xmlRpcMethodActivateLinkParamset(&s);
	static XmlRpcMethodInit xmlRpcMethodInit(&s);
	static XmlRpcMethodSetInstallMode xmlRpcMethodSetInstallMode(&s);
	static XmlRpcMethodGetInstallMode xmlRpcMethodGetInstallMode(&s);
	static XmlRpcMethodDeleteDevice xmlRpcMethodDeleteDevice(&s);
	static XmlRpcMethodGetParamsetId xmlRpcMethodGetParamsetId(&s);
	static XmlRpcMethodReportValueUsage xmlRpcMethodReportValueUsage(&s);
	static XmlRpcMethodAddDevice xmlRpcMethodAddDevice(&s);
	static XmlRpcMethodChangeKey xmlRpcMethodChangeKey(&s);
	static XmlRpcMethodSetTempKey xmlRpcMethodSetTempKey(&s);
	static XmlRpcMethodDetermineParameter xmlRpcMethodDetermineParameter(&s);
	static XmlRpcMethodRestoreConfigToDevice xmlRpcMethodRestoreConfigToDevice(&s);
	static XmlRpcMethodGetKeyMismatchDevice xmlRpcMethodGetKeyMismatchDevice(&s);
	static XmlRpcMethodLogLevel xmlRpcMethodLogLevel(&s);
	static XmlRpcMethodSetTeam xmlRpcMethodSetTeam(&s);
	static XmlRpcMethodRSSIInfo xmlRpcMethodRSSIInfo(&s);
	static XmlRpcMethodListBidcosInterfaces xmlRpcMethodListBidcosInterfaces(&s);
	static XmlRpcMethodSetBidcosInterface xmlRpcMethodSetBidcosInterface(&s);
	static XmlRpcMethodGetServiceMessages xmlRpcMethodGetServiceMessages(&s);
	static XmlRpcMethodGetMetadata xmlRpcMethodGetMetadata(&s);
	static XmlRpcMethodSetMetadata xmlRpcMethodSetMetadata(&s);
	static XmlRpcMethodGetAllMetadata xmlRpcMethodGetAllMetadata(&s);
	static XmlRpcMethodHasVolatileMetadata xmlRpcMethodHasVolatileMetadata(&s);
	static XmlRpcMethodGetVolatileMetadata xmlRpcMethodHetVolatileMetadata(&s);
	static XmlRpcMethodSetVolatileMetadata xmlRpcMethodSetVolatileMetadata(&s);
	static XmlRpcMethodDeleteVolatileMetadata xmlRpcMethodDeleteVolatileMetadata(&s);		
	static XmlRpcMethodGetVersion xmlRpcMethodGetVersion(&s);
	static XmlRpcMethodSetInterfaceClock xmlRpcMethodSetInterfaceClock(&s);
	static XmlRpcMethodUpdateFirmware xmlRpcMethodUpdateFirmware(&s);
	static XmlRpcMethodReplaceDevice xmlRpcMethodReplaceDevice(&s);
	static XmlRpcMethodAddVirtualDeviceInstance xmlRpcMethodAddVirtualDeviceInstance(&s);
	static XmlRpcMethodListReplaceableDevices xmlRpcMethodListReplaceableDevices(&s);
	static XmlRpcMethodAbortDeleteDevice xmlRpcMethodAbortDeleteDevice(&s);
	static XmlRpcMethodSetRFLGWInfoLED xmlRpcMethodSetRFLGWInfoLED(&s);
	static XmlRpcMethodRefreshDeployedDeviceFirmwareList xmlRpcMethodRefreshDeployedDeviceFirmwareList(&s);
	static XmlRpcMethodPing xmlRpcMethodPing(&s);
}

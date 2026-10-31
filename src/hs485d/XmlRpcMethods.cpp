/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485Manager.h"
#include "XmlRpcMethods.h"
#include <Logger.h>

using namespace XmlRpc;

//! Throws a fault when a method is called with fewer than count arguments
static void RequireParams(XmlRpc::XmlRpcValue& params, int count)
{
	if(params.getType()!=XmlRpcValue::TypeArray || params.size()<count){
		throw XmlRpcException("Insufficient parameters.", -1);
	}
}

extern HS485Manager mgr;

//#define LOG_METHOD_CALLS

#ifdef LOG_METHOD_CALLS
class XmlRpcDebugMethod : public XmlRpcServerMethod  
{
public:
	XmlRpcDebugMethod(const char* name, XmlRpcServer* s):XmlRpcServerMethod(name, s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		LOG(Logger::LOG_DEBUG, "XmlRpcMethod %s(%s)", _name.c_str(), params.toText().c_str());
		do_execute(params, result);
		LOG(Logger::LOG_DEBUG, "XmlRpcMethod %s(%s)return", _name.c_str(), params.toText().c_str());
		LOG(Logger::LOG_DEBUG, "%s", result.toText().c_str());
	}
	virtual void do_execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)=0;
};
#define XmlRpcServerMethod XmlRpcDebugMethod
#define execute(x,y) do_execute(x,y)
#endif

/*
 * Array<DeviceDescription> listDevices()
 *
 * Diese Methode gibt alle dem Schnittstellenproze� bekannten Ger�te in Form von
 * Ger�tebeschreibungen zur�ck. Der Parameter �search� gibt an, ob der entsprechende Bus vorher
 * nach Ger�ten durchsucht werden soll.
 *
 */
class XmlRpcMethodListDevices : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodListDevices(XmlRpcServer* s):XmlRpcServerMethod("listDevices", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!mgr.ListDevices(&result)){
			throw XmlRpcException("Failure");
		}
	}
};

/*
 * int searchDevices()
 *
 * Diese Methode durchsucht den Bus nach neuen Ger�ten und gibt die Anzahl gefundener Ger�te zur�ck
 *
 */
class XmlRpcMethodSearchDevices : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodSearchDevices(XmlRpcServer* s):XmlRpcServerMethod("searchDevices", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		int count=mgr.SearchDevices();
		if(count<0){
			throw XmlRpcException("Failure");
		}
		result=count;
	}
};

/*
 * DeviceDescription getDeviceDescription(String address)
 *
 * Diese Methode gibt die Ger�tebeschreibung des als "address" �bergebenen Ger�tes
 * zur�ck. 
 *
 */
class XmlRpcMethodGetDeviceDescription : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetDeviceDescription(XmlRpcServer* s):XmlRpcServerMethod("getDeviceDescription", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!mgr.GetDeviceDescription(params[0], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * ParamsetDescription getParamsetDescription(String address, String paramset_type)
 *
 * Mit dieser Methode wird die Beschreibung eines Parameter-Sets ermittelt. Der Parameter
 * �address� ist die Adresse eines logischen Ger�tes (z.B. von listDevices zur�ckgegeben). Der
 * Parameter �paramset_type� ist �MASTER�, �VALUES� oder �LINK�.
 *
 */
class XmlRpcMethodGetParamsetDescription : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetParamsetDescription(XmlRpcServer* s):XmlRpcServerMethod("getParamsetDescription", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 2);
		result.assertStruct();
		if(!mgr.GetParamsetDescription((std::string&)params[0], (std::string&)params[1], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

/*
 * String getParamsetId(String address, String type)
 *
 * Diese Methode gibt die Id eines Parametersets zur�ck
 *
 */
class XmlRpcMethodGetParamsetId : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetParamsetId(XmlRpcServer* s):XmlRpcServerMethod("getParamsetId", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 2);
		std::string& id=result;
		if(!HS485Manager::GetSingleton()->GetParamsetId(params[0], params[1], &id)){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * void putParamset(String address, String paramset_key, Paramset set)
 *
 * Mit dieser Methode wird ein komplettes Parameter-Set f�r ein logisches Ger�t geschrieben. Der
 * Parameter �address� ist die Addresses eines logischen Ger�tes. Der Parameter
 * �paramset_key� ist �MASTER�, �VALUES� oder die Adresse eines Kommunikationspartners
 * f�r das entsprechende Link-Parameter-Set (siehe getLinkPeers).
 * Der Parameter �set� ist das zu schreibende Parameter-Set. In �set� nicht vorhandene Member
 * werden einfach nicht geschrieben und behalten ihren alten Wert.
 *
 */
class XmlRpcMethodPutParamset : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodPutParamset(XmlRpcServer* s):XmlRpcServerMethod("putParamset", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 3);
		if(!mgr.PutParamsetValues((std::string&)params[0], (std::string&)params[1], params[2])){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * Paramset getParamset(String address, String paramset_key)
 *
 * Mit dieser Methode wird ein komplettes Parameter-Set f�r ein logisches Ger�t gelesen. Der
 * Parameter �address� ist die Addresses eines logischen Ger�tes. Der Parameter
 * �paramset_key� ist �MASTER�, �VALUES� oder die Adresse eines Kommunikationspartners
 * f�r das entsprechende Link-Parameter-Set (siehe getLinkPeers). 
 *
 */
class XmlRpcMethodGetParamset : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetParamset(XmlRpcServer* s):XmlRpcServerMethod("getParamset", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 2);
		result.assertStruct();
		if(!mgr.GetParamsetValues((std::string&)params[0], (std::string&)params[1], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

/*
 * DeviceDescription deleteDevice(String address, Integer flags)
 *
 * Diese Methode l�scht ein angelerntes Ger�t
 *
 * flags ist ein bitweises oder folgender Werte:
 * 0x01 (DELETE_FLAG_RESET) : Das Ger�t wird vor dem L�schen in den Werkszustand zur�ckgesetzt
 * 0x02 (DELETE_FLAG_FORCE) : Das Ger�t wird auch gel�scht, wenn es nicht erreichbar ist
 * 0x04 (DELETE_FLAG_DEFER) : Wenn das Ger�t nicht erreichbar ist, wird es bei n�chster Gelegenheit gel�scht
 */
class XmlRpcMethodDeleteDevice : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodDeleteDevice(XmlRpcServer* s):XmlRpcServerMethod("deleteDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		int flags=0;
		if(params[1].getType()==XmlRpcValue::TypeBoolean && (bool&)params[1])flags|=HS485Manager::DELETE_FLAG_RESET;
		if(params[2].getType()==XmlRpcValue::TypeBoolean && (bool&)params[2])flags|=HS485Manager::DELETE_FLAG_FORCE;
		if(params[1].getType()==XmlRpcValue::TypeInt)flags=params[1];
		if(!HS485Manager::GetSingleton()->DeleteDevice(params[0], flags)){
			throw XmlRpcException("Failure");
		}
	}
};


/* 
 * ValueType getValue(String address, String value_key)
 *
 * Mit dieser Methode wird ein einzelner Wert aus dem Parameter-Set �VALUES� gelesen. Der
 * Parameter �address� ist die Addresse eines logischen Ger�tes. Der Parameter �value_key� ist
 * der Name des zu lesenden Wertes. Die m�glichen Werte f�r value_key ergeben sich aus der
 * ParamsetDescription des entsprechenden Parameter-Sets �VALUES�. 
 *
 */
class XmlRpcMethodGetValue : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetValue(XmlRpcServer* s):XmlRpcServerMethod("getValue", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 2);
		if(!mgr.GetValue(params[0], params[1], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * ValueType setValue(String address, String value_key)
 *
 * Mit dieser Methode wird ein einzelner Wert aus dem Parameter-Set �VALUES� geschrieben. Der
 * Parameter �address� ist die Addresse eines logischen Ger�tes. Der Parameter �value_key� ist
 * der Name des zu schreibenden Wertes. Die m�glichen Werte f�r value_key ergeben sich aus der
 * ParamsetDescription des entsprechenden Parameter-Sets �VALUES�. 
 *
 */
class XmlRpcMethodSetValue : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodSetValue(XmlRpcServer* s):XmlRpcServerMethod("setValue", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 3);
		if(!mgr.SetValue(params[0], params[1], params[2])){
			throw XmlRpcException("Failure");
		}
	}
};


/* 
 * Array<String>getLinkPeers(String address)
 *
 * Diese Methode gibt alle einem logischen Ger�t zugeordneten Kommunikationspartner zur�ck. Die
 * zur�ckgegebenen Werte k�nnen als Parameter �paramset_key� f�r getParamset und
 * putParamset verwendet werden. Der Parameter �address� ist die Addresses eines logischen
 * Ger�tes. 
 */
class XmlRpcMethodGetLinkPeers : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetLinkPeers(XmlRpcServer* s):XmlRpcServerMethod("getLinkPeers", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		std::vector<std::string> peers;
		if(!HS485Manager::GetSingleton()->GetLinkPeers((std::string&)params[0], &peers)){
			throw XmlRpcException("Failure");
		}
		result.assertArray(peers.size());
		for(unsigned int i=0;i<peers.size();i++){
			result[i]=peers[i];
		}
	}
};

/* 
 * Array<Struct>getLinks(String address, Integer flags)
 *
 * Diese Methode gibt alle einem logischen Kanal oder Ger�t zugeordneten Kommunikationsbeziehungen zur�ck.
 * address ist die Kanal- oder Ger�teadresse des logischen Objektes, auf das sich die Abfrage bezieht.
 * Bei address=="" werden alle Kommunikationsbeziehungen des Schnittstellenprozesses zur�ckgegeben
 *
 * flags ist ein bitweises oder folgender Werte:
 * 0x01 (GL_FLAG_GROUP) : wenn address einen Kanal bezeichnet, der sich in einer Gruppe befindet, werden die
 *                     Kommunikationsbeziehungen f�r alle Kan�le der Gruppe zur�ckgegeben
 * 0x02 (GL_FLAG_SENDER_PARAMSET) : Das Feld SENDER_PARAMSET des R�ckgabewertes wird gef�llt
 * 0x04 (GL_FLAG_RECEIVER_PARAMSET) : Das Feld RECEIVER_PARAMSET des R�ckgabewertes wird gef�llt
 * 0x08 (GL_FLAG_SENDER_DESCRIPTION) : Das Feld SENDER_DESCRIPTION des R�ckgabewertes wird gef�llt
 * 0x10 (GL_FLAG_RECEIVER_DESCRIPTION) : Das Feld RECEIVER_DESCRIPTION des R�ckgabewertes wird gef�llt
 * flags ist optional. Defaultwert ist 0x00.
 *
 * Der R�ckgabewert ist ein Array von Strukturen. Jede dieser Strukturen enth�lt die folgenden Felder:
 * String SENDER : Adresse des Senders der Kommunikationsbeziehung
 * String RECEIVER : Adresse des Empf�ngers der Kommunikationsbeziehung
 * String NAME : Name der Kommunikationsbeziehung
 * String DESCRIPTION : Beschreibung der Kommunikationsbeziehung
 * Paramset SENDER_PARAMSET : Parametersatz dieser Kommunikationsbeziehung f�r die Senderseite
 * Paramset RECEIVER_PARAMSET : Parametersatz dieser Kommunikationsbeziehung f�r die Empf�ngerseite
 * DeviceDescription SENDER_DESCRIPTION : Ger�tebeschreibung des Senders
 * DeviceDescription RECEIVER_DESCRIPTION : Ger�tebeschreibung des Empf�ngers
 */

class XmlRpcMethodGetLinks : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetLinks(XmlRpcServer* s):XmlRpcServerMethod("getLinks", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		result.assertArray(0);
		params[1];
		if(!mgr.GetLinks((std::string&)params[0], (int&)params[1], &result)){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * void addLink(String sender_address, String receiver_address, String name, String description)
 *
 * Diese Methode erstellt eine Kommunikationsbeziehung zwischen zwei logischen Ger�ten. Die
 * Parameter �sender_address� und �receiver_address� bezeichnen die beiden zu verkn�pfenden Partner.
 *
 */
class XmlRpcMethodAddLink : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodAddLink(XmlRpcServer* s):XmlRpcServerMethod("addLink", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[1];
		if(!mgr.AddLink((std::string&)params[0], (std::string&)params[1])){
			throw XmlRpcException("Failure");
		}
		if(params.size()>=3){
			params[3];
			if(!mgr.SetLinkInfo((std::string&)params[0], (std::string&)params[1], (std::string&)params[2], (std::string&)params[3])){
				throw XmlRpcException("Failure");
			}
		}
	}
};

/* 
 * void setLinkInfo(String sender_address, String receiver_address, String name, String description)
 *
 * Diese Methode erstellt eine Kommunikationsbeziehung zwischen zwei logischen Ger�ten. Die
 * Parameter �sender_address� und �receiver_address� bezeichnen die beiden zu verkn�pfenden Partner.
 *
 */
class XmlRpcMethodSetLinkInfo : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodSetLinkInfo(XmlRpcServer* s):XmlRpcServerMethod("setLinkInfo", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[3];
		if(!mgr.SetLinkInfo((std::string&)params[0], (std::string&)params[1], (std::string&)params[2], (std::string&)params[3])){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * Array<String> getLinkInfo(String sender_address, String receiver_address)
 *
 * Diese Methode gibt den Namen und die Beschreibung f�r eine erstellt eine Kommunikationsbeziehung zur�ck. Die
 * Parameter "sender_address" und "receiver_address" bezeichnen die beiden verkn�pften Partner.
 *
 */
class XmlRpcMethodGetLinkInfo : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodGetLinkInfo(XmlRpcServer* s):XmlRpcServerMethod("getLinkInfo", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[1];
		result["NAME"];
		result["DESCRIPTION"];
		if(!HS485Manager::GetSingleton()->GetLinkInfo((std::string&)params[0], (std::string&)params[1], &((std::string&)result["NAME"]), &((std::string&)result["DESCRIPTION"]))){
			throw XmlRpcException("Failure");
		}

	}
};

/* 
 * void removeLink(String sender_address, String receiver_address)
 *
 * Diese Methode l�scht eine Kommunikationsbeziehung aus einem logischen Ger�t. Der Parameter
 * �address� ist die Addresse eines logischen Ger�tes. Der Parameter �peer_address� ist die
 * Addresse des Ger�tes dessen Kommunikationszuordnung gel�scht werden soll. 
 *
 */
class XmlRpcMethodRemoveLink : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodRemoveLink(XmlRpcServer* s):XmlRpcServerMethod("removeLink", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		RequireParams(params, 2);
		if(!mgr.RemoveLink((std::string&)params[0], (std::string&)params[1])){
			throw XmlRpcException("Failure");
		}
	}
};

/* 
 * void clearConfigCache(String address)
 *
 * Diese Methode l�scht alle zu einem Ger�t in der Zentrale gespeicherten Konfigurationsdaten
 *
 */
class XmlRpcMethodClearConfigCache : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodClearConfigCache(XmlRpcServer* s):XmlRpcServerMethod("clearConfigCache", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!mgr.ClearConfigCache((std::string&)params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

/*
 * void restoreConfigToDevice(String address)
 *
 * Sends all configuration data of a device stored on the CCU to the
 * device again (same as restoreConfigToDevice of rfd).
 */
class XmlRpcMethodRestoreConfigToDevice : public XmlRpcServerMethod
{
public:
	XmlRpcMethodRestoreConfigToDevice(XmlRpcServer* s):XmlRpcServerMethod("restoreConfigToDevice", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if(!mgr.RestoreConfigToDevice((std::string&)params[0])){
			throw XmlRpcException("Failure");
		}
	}
};

#if 0
/*
 * void activateLinkParamset(String address, String peer_address, Boolean long_press)
 *
 * Mit dieser Methode wird ein Link-Parameterset aktiviert. Das logische Ger�t verh�lt sich dann so
 * als ob es direkt von dem entsprechenden zugeordneten Ger�t angesteuert worden w�re. Hiermit
 * kann z.B. ein Link-Parameter-Set getestet werden. Der Parameter "address" ist die Addresses des
 * anzusprechenden logischen Ger�tes. Der Parameter "peer_address" ist die Addresse des
 * Kommunikationspartners, dessen Link-Parameter-Set aktiviert werden soll.
 * Der Parameter "long_press" gibt an, ob das Parameterset f�r den langen Tastendruck aktiviert
 * werden soll.
 *
 */
class XmlRpcMethodActivateLinkParamset : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodActivateLinkParamset(XmlRpcServer* s):XmlRpcServerMethod("activateLinkParamset", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[2];
		if(!HS485Manager::GetSingleton()->ActivateLinkParamset((std::string&)params[0], (std::string&)params[1], (bool&)params[2])){
			throw XmlRpcException("Failure");
		}
	}
};
#endif

/*
 * void init(String ise_url, String interface_id, bool no_callback = false)
 *
 * Mit dieser Methode teilt die Basisplattform dem Schnittstellenproze� mit, da� sie gerade gestartet
 * wurde. Der Schnittstellenproze� wird sich daraufhin selbst initialisieren und z.B. mit listDevices()
 * die der Basisplattform bekannten Ger�te abfragen.
 * Der Parameter �ise_url� gibt die Adresse des XmlRpc-Servers an, unter der die Basisplattform
 * zu erreichen ist.
 * Der Parameter �interface_id� teilt dem Schnittstellenproze� die Id mit unter der er sich
 * gegen�ber der Basisplattform identifiziert.
 * Mit dem Parameter �no_callback� wird der Aufruf von Callback-Funktionen beim Aufrufer
 * unterdr�ckt.
 *
 */
class XmlRpcMethodInit : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodInit(XmlRpcServer* s):XmlRpcServerMethod("init", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		params[2];
		HS485Manager::GetSingleton()->PlatformInit(params[0], params[1], params[2]);
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
		for(int i=0;i<params.size();i++){
			(bool&)result[i]=HS485Manager::GetSingleton()->UpdateFirmware(params[i]);

		}
	}
};

/*
 * Boolean reportValueUsage(String address, String value_id, Integer ref_counter)
 *
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
	XmlRpcMethodReportValueUsage(XmlRpcServer* s):XmlRpcServerMethod("reportValueUsage", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		LOG(Logger::LOG_DEBUG, "ReportValueUsage(%s)", params.toText().c_str());
		params[2];
		(bool&)result=HS485Manager::GetSingleton()->ReportValueUsage(params[0], params[1], params[2]);
	}
};

/*
 * int logLevel([optional]int level)
 *
 * Diese Methode gibt den aktuellen Log-Level zur�ck bzw. setzt diesen wenn der optionale Parameter level
 * �bergeben wird.
 *
 */
class XmlRpcMethodLogLevel : public XmlRpcServerMethod  
{
public:
	XmlRpcMethodLogLevel(XmlRpcServer* s):XmlRpcServerMethod("logLevel", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		if ((params.getType() == XmlRpc::XmlRpcValue::TypeArray) && (params.size()>=1)) {
			logger->SetLevel((Logger::LogLevel)((int&)params[0]));
		}
		result=logger->GetLevel();
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
		(bool&)result = HS485Manager::GetSingleton()->ReplaceDevice(params[0],params[1]);

	}
};



class XmlRpcMethodGetLGWStatus : public XmlRpcServerMethod
{
public:
	XmlRpcMethodGetLGWStatus(XmlRpcServer* s):XmlRpcServerMethod("getLGWStatus", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		std::string serial;
		bool connected = false;
		HS485Manager::GetSingleton()->GetLGWStatus(&serial, &connected);
		(std::string&)result["SERIAL"] = serial;
		(bool&)result["CONNECTED"] = connected;
	}
};

class XmlRpcMethodListReplaceableDevices : public XmlRpcServerMethod
{
public:
	XmlRpcMethodListReplaceableDevices(XmlRpcServer* s):XmlRpcServerMethod("listReplaceableDevices", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		bool retVal = false;
		if (params.size() == 1 || params.size() == 2) {//TWIST-768: Support but ignore additional parameter for compatibility to rfd method signature
			retVal = HS485Manager::GetSingleton()->ListReplaceableDevices(params[0], &result);
			if(!retVal) {
				throw XmlRpcException("Failure");
			}
		} else {
			throw XmlRpcException("Failure. Wrong parameter.");
		}
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
			HS485Manager::GetSingleton()->Ping(params[0]);
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
	static XmlRpcMethodSearchDevices xmlRpcMethodSearchDevices(&s);
	static XmlRpcMethodGetDeviceDescription xmlRpcMethodGetDeviceDescription(&s);
	static XmlRpcMethodGetLinkPeers xmlRpcMethodGetLinkPeers(&s);
	static XmlRpcMethodGetLinks xmlRpcMethodGetLinks(&s);
	static XmlRpcMethodAddLink xmlRpcMethodAddLink(&s);
	static XmlRpcMethodSetLinkInfo xmlRpcMethodSetLinkInfo(&s);
	static XmlRpcMethodGetLinkInfo xmlRpcMethodGetLinkInfo(&s);
	static XmlRpcMethodGetParamsetId xmlRpcMethodGetParamsetId(&s);
	static XmlRpcMethodRemoveLink xmlRpcMethodRemoveLink(&s);
//	static XmlRpcMethodActivateLinkParamset xmlRpcMethodActivateLinkParamset(&s);
	static XmlRpcMethodInit xmlRpcMethodInit(&s);
	static XmlRpcMethodUpdateFirmware xmlRpcMethodUpdateFirmware(&s);
	static XmlRpcMethodDeleteDevice xmlRpcMethodDeleteDevice(&s);
	static XmlRpcMethodReportValueUsage xmlRpcMethodReportValueUsage(&s);
	static XmlRpcMethodClearConfigCache xmlRpcMethodClearConfigCache(&s);
	static XmlRpcMethodRestoreConfigToDevice xmlRpcMethodRestoreConfigToDevice(&s);
	static XmlRpcMethodLogLevel xmlRpcMethodLogLevel(&s);
	static XmlRpcMethodReplaceDevice XmlRpcMethodReplaceDevice(&s);
	static XmlRpcMethodListReplaceableDevices xmlRpcMethodListReplaceableDevices(&s);
	static XmlRpcMethodGetLGWStatus xmlRpcMethodGetLGWStatus(&s);
	static XmlRpcMethodPing xmlRpcMethodPing(&s);
}

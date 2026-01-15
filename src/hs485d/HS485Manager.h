/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_MANAGER_H_
#define _HS485_MANAGER_H_

#include "HS485Device.h"
#include "HS485SystemDescription.h"
#include "HS485Paramset.h"
#include "HS485FirmwareManager.h"

#include <HSSManager.h>
#include <map>
#include <string>

//! Zentrale Verwaltungsklasse
/*!
 *  An dieser Klasse hängen die Geräteobjekte. Sie übernimmt folgende Aufgaben:
 *  - Laden der persistierten Geräte beim Starten
 *  - Anlernen neuer Geräte
 *  - Verteilung der XmlRpc-Aufrufe auf die Geräte und Kanäle
 *  - Verteilung von der Busseite her eingehender Nachrichten an die Geräte
 *  - Verteilung von Ereignissen an die Logikprozesse (siehe HSSManager)
 */
class HS485Manager : public HSSManager
{
public:
	enum{
		DELETE_FLAG_RESET=(1<<0),
		DELETE_FLAG_FORCE=(1<<1),
		DELETE_FLAG_DEFER=(1<<2)
	};
	HS485Manager(void);
	virtual ~HS485Manager(void);
	bool GetParamsetValues(const std::string address, const std::string& key, XmlRpc::XmlRpcValue* set);
	bool PutParamsetValues(const std::string address, const std::string& key, XmlRpc::XmlRpcValue& set);
	bool GetParamsetDescription(const std::string address, const std::string& key, XmlRpc::XmlRpcValue* set);
	bool GetParamsetId(const std::string address, const std::string& type, std::string* id);

	bool GetValue(const std::string address, const std::string& name, XmlRpc::XmlRpcValue* val);
	bool SetValue(const std::string address, const std::string& name, XmlRpc::XmlRpcValue& val);

	bool ListDevices(XmlRpc::XmlRpcValue* devs);

	int SearchDevices();

	bool GetLinkPeers(const std::string& address, std::vector<std::string>* peers);
	bool GetLinks(const std::string& address, int flags, XmlRpc::XmlRpcValue* result);
	bool AddLink(const std::string& sender_address, const std::string& receiver_address);
	bool SetLinkInfo(const std::string& sender_address, const std::string& receiver_address, const std::string& name, const std::string& description);
	bool GetLinkInfo(const std::string& sender_address, const std::string& receiver_address, std::string* name, std::string* description);
	bool RemoveLink(const std::string& sender_address, const std::string& receiver_address);
	void ProcessIncomingFrame(HS485Frame& frame);
	using HSSManager::Init;
	bool Init(const std::string& configfilePath);
	static HS485Manager* GetSingleton(){
		return singleton;
	}

	std::string BuildStringAddress(uint32_t address, int channel=-1);
	static std::string BuildStringAddress(const std::string&  address, int channel=-1);
	bool ParseAddress(const std::string& address, uint32_t* dev_address, int * channel);
	bool ParseAddress(const std::string& address, std::string * dev_address, int * channel);
	void ReportNewDevice(HS485LogicalInstance* dev);
	void ReportDeletedDevice(HS485Device* dev);
	bool GetDeviceDescription(const std::string& address, XmlRpc::XmlRpcValue* descr);
	bool UpdateFirmware(const std::string& address);
	bool DeleteDevice(const std::string& address, int flags);
	HS485SystemDescription* GetSystemDescription(){return &system_description;};
	HS485LogicalInstance* GetInstance(const std::string& address);
	bool ReportValueUsage(const std::string& address, const std::string& value, int count);
	HS485Device* GetDeviceByAddress(uint32_t address);
	HS485FirmwareManager* GetFirmwareManager(){return &fw_mgr;};
	bool ClearConfigCache(const std::string& address);
	bool ActivateLinkParamset(const std::string address, const std::string& peer, bool longpress);


	/**\brief Checks wether a device has been replaced or not.
	 * \param oldDeviceSerial Serial number of the old/original device.
	 * \param newDeviceSerial Serial number of the replacement device, if it has been replaced.
	 * \return True if oldDevice had been replaced, otherwise False.*/
	virtual bool IsDeviceReplaced(const std::string &oldDeviceSerial, std::string &newDeviceSerial);

	/**\brief Replaces a (broken) device with a new device.
	 * \param oldDeviceSerial Serial number of device to be replace.
	 * \param newDeviceSerial Serial number of device to replace the old one.
	 * \return True on success, otherwise False.*/
	bool ReplaceDevice(const std::string& oldDeviceSerial, const std::string& newDeviceSerial);

	void ReportDeviceReplacement(const std::string oldDevSerial, const std::string newDevSerial);

	bool ListReplaceableDevices(const std::string& devSerial, XmlRpc::XmlRpcValue* devs);

	bool GetLGWStatus(std::string* serial, bool* connected);

	const std::string& GetDeviceFilesPath() const;

protected:
	bool AddNewDevice(unsigned int address, HS485Frame* add_frame=NULL);
	bool CreateDevices(XmlRpc::XmlRpcValue& devs);
	bool SaveDeviceList();
	bool LoadDeviceList();
	HS485SystemDescription system_description;
	typedef std::map<std::string, HS485Device*> t_dev_instances;
	t_dev_instances dev_instances;
	typedef std::map<uint32_t, HS485Device*> t_address_map;
	t_address_map address_map;
	static HS485Manager* singleton;
	HS485FirmwareManager fw_mgr;
	std::string deviceFilesPath;

	/** \brief Map that holds new HS485Device instance for serial numbers that have been replaced.
	 * \details Key: Serial number of devices, that have been replaced. Value: Dev-Instance of the new device.
	 */
	std::map<std::string, HS485Device*> replacementHistory;

	void initReplacementHistory();


};
#endif //_HS485_MANAGER_H_

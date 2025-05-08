// HS485Device.h: Schnittstelle für die Klasse HS485Device.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485DEVICE_H__31222142_578A_4D7D_917B_5F9694706BFF__INCLUDED_)
#define AFX_HS485DEVICE_H__31222142_578A_4D7D_917B_5F9694706BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HS485EEpromCache.h"
#include "HS485Paramset.h"
#include "HS485Channel.h"
#include "HS485LogicalInstance.h"
#include <TimerTarget.h>
#include <vector>
#include <unistd.h>

//! Jede Instanz dieser Klasse kümmert sich um ein konkretes angelerntes BidCoS-Wired-Gerät
/*! Diese Klasse verwaltet die Informationen, die für ein konkretes Gerät zutreffen. Dies sind
 *  Zustandsinformationen und Adressierungsinformationen. Informationen, die eine Geräteklasse
 *  betreffen (also das, was aus der XML-Datei gelesen wird), werden von HS485DeviceDescription
 *  verwaltet. Jede Instanz von HS485Device enthält einen Zeiger auf die zugehörige Instanz von
 *  HS485DeviceDescription.
 *  Die Gerätekanäle werden analog dazu von HS485Channel / HS485ChannelDescription verwaltet. Jede
 *  Instanz von HS485Device enthält dazu eine Map von Instanzen von HS485Channel und HS485DeviceDescription
 *  enthält einen Vektor mit Instanzen von HS485ChannelDescription.
 */
class HS485Device : public HS485LogicalInstance
{
public:
	enum{
		FLAG_UNREACH=(1<<0),
		FLAG_STICKY_UNREACH=(1<<1),
	};
	enum{
		FAIL_COUNTER_RESET_TIME=5000
	};
    typedef std::vector<unsigned char> data_t;
	bool GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set);
	bool PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set);
	bool GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set);
	bool GetParamsetId(const std::string& key, std::string* id);

	bool GetValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool SetValue(const std::string& name, XmlRpc::XmlRpcValue& val);

	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool IsConfigPending();

	HS485EEPromCache* GetEEPromCache();
	bool SendMessage(const std::string& msg, std::string* response);
	virtual bool SendMessage(HS485CommMessage* msg);
	void ProcessIncomingFrame(HS485Frame& frame);

	bool AddChannel(int index, const std::string& type);
	void SetDeviceDescription(HS485DeviceDescription* description, bool add_channels=true);
	inline void SetAddress(unsigned long address){this->address=address;};
	inline unsigned long GetAddress(){return this->address;};
	bool Describe(XmlRpc::XmlRpcValue* val);
	bool SetEnforcedParameters();
	HS485LogicalInstance* GetInstance(int channel_index);
	virtual HS485Device* GetDevice(){return this;};
	std::vector<int> ListChannels();
	bool FactoryReset();
	bool UnpeerCentral();
	virtual bool SaveToXml(XMLNode* node);
	virtual bool LoadFromXml(XMLNode& node);
	virtual bool Save();
	virtual void SetSerial(const std::string s){serial=s;};
	virtual void SetType(const std::string t){type=t;};
	const std::string& GetSerial(){return serial;};
	const std::string& GetType(){return type;};
	const std::string& GetAvailableFirmware();
	bool DetermineSerial(bool has_serial);
	bool Delete();
	bool GetLinks(int flags, link_map_t* result);
	void SetSysinfo(const std::string& si);
	HS485DeviceDescription* GetDeviceDescription(){return description;};
	HS485Device(unsigned int address=0);
	HS485Device(const HS485Device & inst);
	virtual ~HS485Device();
	HS485Channel* GetChannelByPhysicalIndex(int index, int direction);
	void InitNewDevice();
	void ClearConfigCache();
	bool UpdateFirmware();
	bool GetConfig(XmlRpc::XmlRpcValue* c);
	bool RestoreConfig(XmlRpc::XmlRpcValue& c, const bool disableEEPCacheFlushing = true);
	void RequestSave(int timeout=0);

	virtual bool SetDefaultConfig() { return true; }

	/** \brief Checks if given device can replace this device.
	 * \param newDevice Device to replace this with.
	 * \return True if newDevice can replace this device, otherwise False.*/
	bool IsCompatible(const HS485Device* newDevice);

	const std::vector<std::string>& getReplacementHistory() const;

	bool replaceDeviceWith(HS485Device* newDevice);
	bool Rebuild(const bool enforceConfigRestore = false, const bool reportNewDevice = true);
	
		//Method used by rfd (which is pure virtual in class LogicalInstance)
	virtual void SetValueAsDefined(const std::string&) {}
	//Method used by rfd (which is pure virtual in class LogicalInstance)
	virtual void SetValueAsUndefined(const std::string&) {}
protected:
	enum{TIMER_RESET_FAIL_COUNTER=1000, TIMER_SAVE};

	bool WriteFlash(const std::vector<unsigned char> &buffer, unsigned int start);
	bool VerifyFlash(const std::vector<unsigned char> &buffer, unsigned int start);
	void OnTimer(uint32_t cookie);
	void CheckConfigPendingEvent();
	void ClearChannels();
    bool ReadEEProm(unsigned int address, unsigned int count, data_t* data);
    bool WriteEEProm(unsigned int address, const data_t& data);
	bool GetEEPromUsage(unsigned int address, unsigned int block_size, unsigned int count, data_t* used_bits);
	virtual inline HS485Channel* CreateChannel(){return new HS485Channel();};
    unsigned long address;
	std::string serial;
    HS485EEPromCache eep_cache;
	HS485DeviceDescription* description;
	typedef std::map<int, HS485Channel*> channels_t;
	channels_t channels;
	std::string type;
	bool last_config_pending;
	unsigned long maintenance_flags;
	std::string firmware_version;
	std::string sysinfo;
	std::string available_firmware;
	int fail_counter;
    friend class HS485EEPromCache;
	friend class HS485Channel;

	/** \brief Vector that contains replacement history of this device (serial numbers of previous devices).*/
	std::vector<std::string> replacementHistory;
};

#endif // !defined(AFX_HS485DEVICE_H__31222142_578A_4D7D_917B_5F9694706BFF__INCLUDED_)

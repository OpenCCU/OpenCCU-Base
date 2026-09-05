/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <Logger.h>

#include <XmlRpc.h>
#include "RFManager.h"

#include "BidcosInterface.h"
#include "BidcosInterfaceConcentrator.h"
#include "RFController.h"
#include "BidcosLanInterface.h"
#ifdef HAS_USB_SUPPORT
#include "BidcosUsbInterface.h"
#endif
#include "CCU2BidcosRemoteInterface.h"

/*static*/ BidcosInterface* BidcosInterface::CreateFromType(const std::string& type)
{
    if(type=="CCU")return new RFController();
    if(type=="Lan Interface")return new BidcosLanInterface();
#ifdef HAS_USB_SUPPORT
    if(type=="USB Interface")return new BidcosUsbInterface();
#endif
	if(type=="CCU2") return new HM2::CCU2BidcosRemoteInterface(HM2::CCU2BidcosRemoteInterface::CCU2);
	if(type=="HMLGW2") return new HM2::CCU2BidcosRemoteInterface(HM2::CCU2BidcosRemoteInterface::HMLGW2);
	if(type=="HM-MOD-UART") return new HM2::CCU2BidcosRemoteInterface(HM2::CCU2BidcosRemoteInterface::HM_MOD_UART);
    return NULL;
}

using namespace XmlRpc;

static const int DUTYCYCLE_THRESHOLDMAP[] = 
{
	0,
	40,
	65,
	80,
	90,
	95,
	98,
	99,
	100
};

static const int DUTYCYCLE_THRESHOLDMAP_SIZE = (sizeof(DUTYCYCLE_THRESHOLDMAP) / sizeof(DUTYCYCLE_THRESHOLDMAP[0]));

BidcosInterface::BidcosInterface(void)
{
	concentrator=NULL;
	pthread_mutex_init(&mutex_devices, NULL);

	duty_cycle = 0;
	duty_cycle_threshold = 0;
}

BidcosInterface::~BidcosInterface(void)
{
	pthread_mutex_destroy(&mutex_devices);
}

BidcosInterface::DeviceData::DeviceData()
{
	aes_key=0;
	aes_channels=0;
	flags=0;
	address=0;
}

BidcosInterface::DeviceData::~DeviceData()
{
}

void BidcosInterface::SetConcentrator(BidcosInterfaceConcentrator* ic)
{
	concentrator=ic;
}

BidcosInterfaceConcentrator* BidcosInterface::GetConcentrator()
{
	return concentrator;
}

bool BidcosInterface::AddDevice(int address)
{
	pthread_mutex_lock(&mutex_devices);
	map_devices[address].address=address;
	pthread_mutex_unlock(&mutex_devices);
	return true;
}

bool BidcosInterface::RemoveDevice(int address)
{
	bool success=false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it!=map_devices.end()){
		map_devices.erase(it);
		success=true;
	}
	pthread_mutex_unlock(&mutex_devices);
	return success;
}

void BidcosInterface::ListDevices(BidcosInterface::t_device_list* devices)
{
	pthread_mutex_lock(&mutex_devices);
	for(t_map_devices::iterator it=map_devices.begin();it!=map_devices.end();it++)devices->push_back(it->second.address);
	pthread_mutex_unlock(&mutex_devices);
}

bool BidcosInterface::NeedsWakeup(int address)
{
	bool retval=false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it!=map_devices.end()){
        if(it->second.flags&DeviceData::FLAG_WAKEUP)retval=true;
	}
	pthread_mutex_unlock(&mutex_devices);
	return retval;
}
bool BidcosInterface::NeedsLazyConfig(int address)
{
    bool retval = false;
    pthread_mutex_lock(&mutex_devices);
    t_map_devices::iterator it = map_devices.find(address);
    if (it != map_devices.end())
    {
        if (it->second.flags & DeviceData::FLAG_LAZY_CONFIG)
            retval = true;
    }
    pthread_mutex_unlock(&mutex_devices);
    return retval;
}
void BidcosInterface::ListWakeupDevices(BidcosInterface::t_device_list* devices)
{
	pthread_mutex_lock(&mutex_devices);
    for(t_map_devices::iterator it=map_devices.begin();it!=map_devices.end();it++)if(it->second.flags&DeviceData::FLAG_WAKEUP)devices->push_back(it->second.address);
	pthread_mutex_unlock(&mutex_devices);
}

bool BidcosInterface::SetDeviceAesPolicy(int address, int aes_key, uint64 aes_channels)
{
	bool success=false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it!=map_devices.end()){
		it->second.aes_key=aes_key;
		it->second.aes_channels=aes_channels;
		success=true;
	}
	pthread_mutex_unlock(&mutex_devices);
	return success;
}

bool BidcosInterface::AddDeviceWakeupRequest(int address,bool lazyConfig)
{
	bool success=false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it!=map_devices.end()){
		it->second.flags |= DeviceData::FLAG_WAKEUP;
		if(lazyConfig)
		{
		    it->second.flags |= DeviceData::FLAG_LAZY_CONFIG;
		}
		success=true;
	}
	pthread_mutex_unlock(&mutex_devices);
	return success;
}

bool BidcosInterface::RemoveDeviceWakeupRequest(int address)
{
	bool success=false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it!=map_devices.end()){
		it->second.flags &= ~(DeviceData::FLAG_WAKEUP | DeviceData::FLAG_LAZY_CONFIG);

		success=true;
	}
	pthread_mutex_unlock(&mutex_devices);
	return success;
}

bool BidcosInterface::GetDeviceAesPolicy(int address, int* aes_key, uint64* channels)
{
	bool success=false;
	pthread_mutex_lock(&mutex_devices);
	t_map_devices::iterator it=map_devices.find(address);
	if(it!=map_devices.end()){
		*aes_key=it->second.aes_key;
		*channels=it->second.aes_channels;
		success=true;
	}
	pthread_mutex_unlock(&mutex_devices);
	return success;
}

const std::string& BidcosInterface::GetSerialNumber()
{
	return serial_number;
}

const std::string& BidcosInterface::GetDescription()
{
	return description;
}

bool BidcosInterface::Init(std::map<std::string, std::string>& params)
{
    serial_number=params["Serial Number"];
    if(serial_number.empty())return false;

	iface_type=params["Type"];
    description=params["Description"];
   
	return true;
}

void BidcosInterface::SetFirmwareVersion(const std::string& firmware_version)
{
	this->firmware_version = firmware_version;
}


void BidcosInterface::SetDutyCycle(int new_duty_cycle)
{
	int oldThreshold = duty_cycle_threshold;
	UpdateDutyCycleThreshold(new_duty_cycle);

	duty_cycle = new_duty_cycle;
	if (oldThreshold != duty_cycle_threshold)
	{
		XmlRpcValue value(duty_cycle);
		std::string keyValue = "DUTY_CYCLE";
		RFManager::GetSingleton()->ReportEvent(GetSerialNumber(), keyValue, value);
	}
}

void BidcosInterface::UpdateDutyCycleThreshold(int new_duty_cycle)
{
	if (new_duty_cycle < duty_cycle)
	{
		int lowerBound = GetDutyCycleLowerBound();
		while (new_duty_cycle <= lowerBound && duty_cycle_threshold > 0)
		{ 
			duty_cycle_threshold--;
			lowerBound = GetDutyCycleLowerBound();
		}
	}
	else if (new_duty_cycle > duty_cycle)
	{
		int upperBound = GetDutyCycleUpperBound();
		while (new_duty_cycle >= upperBound && duty_cycle_threshold < DUTYCYCLE_THRESHOLDMAP_SIZE - 1)
		{ 
			duty_cycle_threshold++;
			upperBound = GetDutyCycleUpperBound();
		}
	}
}

int BidcosInterface::GetDutyCycleLowerBound() const
{
	if (duty_cycle_threshold > 0)
	{
		return DUTYCYCLE_THRESHOLDMAP[duty_cycle_threshold - 1];
	}
	else
	{
		return DUTYCYCLE_THRESHOLDMAP[0];
	}
}

int BidcosInterface::GetDutyCycleUpperBound() const
{
	if (duty_cycle_threshold <= DUTYCYCLE_THRESHOLDMAP_SIZE)
	{
		return DUTYCYCLE_THRESHOLDMAP[duty_cycle_threshold + 1];
	}
	else
	{
		return DUTYCYCLE_THRESHOLDMAP[DUTYCYCLE_THRESHOLDMAP_SIZE - 1];
	}
}

int BidcosInterface::GetDutyCycle() const
{
	return this->duty_cycle;
}

const std::string& BidcosInterface::GetFirmwareVersion() const
{
	return this->firmware_version;
}

const std::string& BidcosInterface::GetInterfaceType() const
{
	return this->iface_type;
}

bool BidcosInterface::IsLanInterface() const
{
	return (iface_type == "HMLGW2") || (iface_type == "Lan Interface");
}

bool BidcosInterface::SetInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes)
{
	return true;
}
bool BidcosInterface::Set100kMode()
{
	return false;
}
	
bool BidcosInterface::Set10kMode()
{
	return true;
}

BidcosInterface::DataRate_t BidcosInterface::getDataRate()
{
	return DATA_RATE_10k;
}

bool BidcosInterface::Updateable()
{
	return false;
}
bool BidcosInterface::SupportLazyConfig()
{
    return false;
}

bool BidcosInterface::SetRFLGWInfoLED(const unsigned int /*state*/)
{
	return false;
}

bool BidcosInterface::SupportsTripleBurst() {
	return false;
}

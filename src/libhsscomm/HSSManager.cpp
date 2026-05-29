/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HSSManager.h"
#include "HSSXmlRpcEventMessage.h"
#include <Logger.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <utils.h>
#include "AsyncXmlRpcSender.h"
#include <fstream>
#include <string.h>
#include <OSCompat.h>

static short UDP_PORT=8182;
static const char* UDP_HOST="127.0.0.1";

using namespace XmlRpc;

HSSManager::HSSManager(void)
{
	//initialize UDP socket for communication with display process
    udp_fd = socket(PF_INET, SOCK_DGRAM, 0);

	if(udp_fd<0){
		LOG(Logger::LOG_ERROR, "Could not create UDP socket");
	}
    udp_dest_addr.sin_family = AF_INET;          // host byte order
    udp_dest_addr.sin_port = htons(UDP_PORT);   // short, network byte order
    udp_dest_addr.sin_addr.s_addr = inet_addr(UDP_HOST);
    memset(&(udp_dest_addr.sin_zero), '\0', 8);  // zero the rest of the struct
	multicall=0;
	removeUnreachableClients = true;
	xmlrpcHandlersFilepath = ""; //will be initialized in init method.
}

HSSManager::~HSSManager(void)
{
}

void HSSManager::ReportServiceMessage(const std::string& address, const std::string& value_key, XmlRpc::XmlRpcValue& val)
{
	bool add=false;
	try{
		add=(bool&)val;
	}catch(XmlRpcException&){
		try{
			add=((int&)val)!=0;
		}catch(XmlRpcException&){
			LOG(Logger::LOG_ERROR, "ServiceMessage type conversion failed");
		}
	}
	//LOG(Logger::LOG_DEBUG, "ServiceMessage %s %s.%s=%s", add?"add":"remove", address.c_str(), value_key.c_str(), val.toText().c_str());
	std::string key=address+"|"+value_key;
	if(add){
		service_message_map[key]=val;
	}else{
		service_message_map.erase(key);
	}
	char buffer[8];
	snprintf(buffer, sizeof(buffer), "%zu", service_message_map.size());
	SendUDPInfo("SERVICE", buffer);
}

void HSSManager::SendUDPInfo(const std::string& key, const std::string& value /* ="" */, uint64_t timer /*=0*/)
{
	if(udp_fd<0)return;

//	LOG(Logger::LOG_DEBUG, "SendUDPInfo(%s, %s, %u)", key.c_str(), value.c_str(), timer);
	if(timer){
		std::map<std::string, uint64_t>::iterator it=udp_timer_map.find(key);
		uint64_t now=time_millis();
		if(it!=udp_timer_map.end()){
			if(((int64_t)now-(int64_t)(it->second+timer))<0)return;
		}
		udp_timer_map[key]=now;
	}
	std::string msg=task_id+"\n";
	if(key.length())msg+=key+"=";
	msg+=value+"\n";
	int error=sendto(udp_fd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&udp_dest_addr, sizeof(struct sockaddr));
	if(error<0){
		LOG(Logger::LOG_ERROR, "Could not send UDP packet, error=%d", error);
	}
}

void HSSManager::ReportEvent(const std::string& address, const std::string& value_key, XmlRpc::XmlRpcValue& val)
{
	LOG(Logger::LOG_DEBUG, "Event: %s.%s=%s", address.c_str(), value_key.c_str(), val.toText().c_str());
	XmlRpcValue params;
	params[1]=address;
	params[2]=value_key;
	params[3]=val;
	// XmlRpcCallAsync("event", params);

	// event propagation
	HSSXmlRpcEventMessage message(address, value_key, val);
	t_handler_map::iterator it;
	std::vector<std::string> toDelete; 
	for (it = xmlrpc_handlers.begin(); it != xmlrpc_handlers.end(); ++it)
	{
		HSSXmlRpcEventDispatcher* dispatcher = it->second;

		if((removeUnreachableClients) && (dispatcher->isClientUnreachable())) {//CN: Stability fix. Remove unreachable handlers
			toDelete.push_back(it->first);
			//LOG(Logger::LOG_DEBUG, "Scheduling remove of unreachable XMLRPC handler with url %s.", it->first.c_str());
		}
		else {
			*dispatcher << message;
		}
	}
	for(unsigned int i = 0; i < toDelete.size() ; ++i) 
	{
		HSSXmlRpcEventDispatcher* dispatcher = xmlrpc_handlers[toDelete.at(i)];
		if(dispatcher != NULL) {
			LOG(Logger::LOG_DEBUG, "Removing unreachable XMLRPC handler with url %s.", toDelete.at(i).c_str());
			xmlrpc_handlers.erase(toDelete.at(i));
			delete dispatcher;
		}
	}
	if(!toDelete.empty()) {
		SaveXmlRpcHandlers();
	}
}

bool HSSManager::XmlRpcCallSync(const char* method, XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue* result, const std::string& url)
{
	XmlRpcClient c(url);
	if(!c.execute(method, params, *result)){
		LOG(Logger::LOG_ERROR, "XmlRpc transport error calling %s(%s) on %s:", method, params.toText().c_str(), c.getURL().c_str());
	}else{
		if(c.isFault()){
			LOG(Logger::LOG_ERROR, "XmlRpc fault calling %s(%s) on %s:%s", method, params.toText().c_str(), c.getURL().c_str(), result->toText().c_str());
		}else{
			return true;
		}
	}
	return false;
}

bool HSSManager::XmlRpcCallAsync(const char* method, XmlRpc::XmlRpcValue& params, const std::string& url /*=""*/)
{
	if(multicall && strcmp(method, "system.multicall")!=0 ){
		return StoreMulticall(method, params, url);
	}
	AsyncXmlRpcSender* s;
	t_handler_map::iterator it=xmlrpc_handlers.begin();
	while(it!=xmlrpc_handlers.end()){
		if(url.empty()){
			s=new AsyncXmlRpcSender(it->first);
			params[0]=it->second->GetId();
		}else{
			s=new AsyncXmlRpcSender(url);
		}
		try{
			s->AsyncCall(method, params);
		}catch(XmlRpcException& e){
			LOG(Logger::LOG_DEBUG, "XmlRpc call %s(%s) exception %d (%s)", method, params.toText().c_str(), e.getCode(), e.getMessage().c_str());
		}
		if(!url.empty())break;
		it++;
	}
	return true;
}

bool HSSManager::StoreMulticall(const char* method, XmlRpc::XmlRpcValue& params, const std::string& url /*=""*/)
{
	XmlRpcValue multicall_params;
	multicall_params["methodName"]=method;
	multicall_params["params"]=params;
	if(!url.empty()){
		XmlRpcValue& calls=multicall_map[url];
		int index=0;
		if(calls[0].getType()==XmlRpcValue::TypeArray)index=calls[0].size();
		calls[0][index]=multicall_params;
	}else{
		t_handler_map::iterator it=xmlrpc_handlers.begin();
		while(it!=xmlrpc_handlers.end()){
			multicall_params["params"][0]=it->second->GetId();
			XmlRpcValue& calls=multicall_map[it->first];
			int index=0;
			if(calls[0].getType()==XmlRpcValue::TypeArray)index=calls[0].size();
			calls[0][index]=multicall_params;
			it++;
		}
	}
	return true;
}
bool HSSManager::CommitReplacedDevices(const std::string url,const std::string& id)
{
	XmlRpcValue remote_devs;
	XmlRpcValue local_devs;
	XmlRpcValue params;
	params[0]=id;
    if(XmlRpcCallSync("listDevices", params, &remote_devs, url))
	{
		std::map<std::string, int> remote_version_map;
		for(int i=0;i<remote_devs.size();++i){
			remote_version_map[(std::string&)remote_devs[i]["ADDRESS"]]=(int&)(remote_devs[i]["VERSION"]);
		}

		if(ListDevices(&local_devs))
		{
			
			int j=0;
			params[1].assertArray(0);
			for(int i=0;i<local_devs.size();++i){
				std::map<std::string, int>::iterator it=remote_version_map.find((std::string&)local_devs[i]["ADDRESS"]);
				if(it==remote_version_map.end() || it->second!=(int&)(local_devs[i]["VERSION"])){
					params[1][j++]=local_devs[i];
				}
				if(it!=remote_version_map.end()){
					remote_version_map.erase(it);
				}
			}
			if(params[1].size()){
				XmlRpcCallAsync("newDevices", params, url);
			}
		}

		if(remote_version_map.size()){
			std::map<std::string, int>::iterator it=remote_version_map.begin();
			std::string newDevice;
			for(;it != remote_version_map.end();++it)
			{
				if(IsDeviceReplaced(it->first,newDevice))
				{
					XmlRpcValue params;
					params[0]=id;
					params[1]=it->first;
					params[2] = newDevice;
					XmlRpcCallAsync("replaceDevice", params);
				}

			}
		}
		return true;
	}
	return false;
}
bool HSSManager::PlatformInit(const std::string& url, const std::string& id, bool dont_callback /*=false*/)
{
	LOG(Logger::LOG_DEBUG, "PlatformInit()");
	XmlRpcValue params1;
	params1[0]=id;

	if(id.size()){
		XmlRpcValue remote_Meth;
		XmlRpcCallSync("system.listMethods",params1,&remote_Meth,  url);
		bool supportReplaceDevice = false;
		if(remote_Meth.getType() == XmlRpcValue::TypeArray)
		{
			for(int i = 0; i < remote_Meth.size();++i)
			{
				std::string methname = remote_Meth[i];
				LOG(Logger::LOG_INFO,"%s support %s",id.c_str(), methname.c_str());
				if(methname.compare("replaceDevice")==0)
				{
					supportReplaceDevice = true;
					LOG(Logger::LOG_INFO,"yes support replace device");
				}
			}
		}
		if(xmlrpc_handlers.find(url) == xmlrpc_handlers.end()) {
			HSSXmlRpcEventDispatcher* xmlrpcEventDispatcher = new HSSXmlRpcEventDispatcher(url,id);
			xmlrpc_handlers[url]= xmlrpcEventDispatcher;
			xmlrpcEventDispatcher->Start();
		}
		//tell ISE the devices we know. But for performance sake, 
		//first filter out the devices ISE already knows about.
		XmlRpcValue params;
		params[0]=id;
		XmlRpcValue remote_devs;

		if( (!dont_callback) && XmlRpcCallSync("listDevices", params, &remote_devs, url)){

			//LOG(Logger::LOG_DEBUG, "Remote Devices: %s", remote_devs.toText().c_str());
			

			std::map<std::string, int> remote_version_map;
			for(int i=0;i<remote_devs.size();i++){
				remote_version_map[(std::string&)remote_devs[i]["ADDRESS"]]=(int&)(remote_devs[i]["VERSION"]);
			}

			XmlRpcValue local_devs;

			if(ListDevices(&local_devs)){
				int j=0;
				params[1].assertArray(0);
				for(int i=0;i<local_devs.size();i++){
					std::map<std::string, int>::iterator it=remote_version_map.find((std::string&)local_devs[i]["ADDRESS"]);
					if(it==remote_version_map.end() || it->second!=(int&)(local_devs[i]["VERSION"])){
						params[1][j++]=local_devs[i];
					}
					if(it!=remote_version_map.end()){
						remote_version_map.erase(it);
					}
				}
				if(params[1].size()){
					XmlRpcCallAsync("newDevices", params, url);
				}
			}

			

			if(remote_version_map.size()){
				//we need to delete some remote devices
				XmlRpcValue deleteDevicesParams;
				deleteDevicesParams[0]=id;
                XmlRpcValue& deleteDevicesArray = deleteDevicesParams[1];
                deleteDevicesArray.assertArray(0);

				std::map<std::string, int>::iterator it;
				for(it=remote_version_map.begin();it!=remote_version_map.end();it++){
                    std::string newDevice;
                    if(supportReplaceDevice && IsDeviceReplaced(it->first,newDevice))
                    {
                        XmlRpcValue replaceDeviceParams;
                        replaceDeviceParams[0]=id;
                        replaceDeviceParams[1]=it->first;
                        replaceDeviceParams[2] = newDevice;
                        XmlRpcCallAsync("replaceDevice", replaceDeviceParams);
                    }else{
                        deleteDevicesArray[deleteDevicesArray.size()] = it->first;
                    }
				}
                if( deleteDevicesArray.size() )
                {
                    XmlRpcCallAsync("deleteDevices", deleteDevicesParams, url);
                }
			}
		}
		SendServiceEvents(url, id);
	}else{
		HSSXmlRpcEventDispatcher* dispatcher = xmlrpc_handlers[url];
		xmlrpc_handlers.erase(url);//safer to remove it before deleting the dispatcher
		if (dispatcher)
		{
			LOG(Logger::LOG_DEBUG, "Unregistering %s", url.c_str());
			delete dispatcher;
		}
	}
	SaveXmlRpcHandlers();

	return true;
}

void HSSManager::SendServiceEvents(const std::string& url, const std::string& id)
{
	int i=0;
	MulticallCollectBegin();
	for(t_service_message_map::iterator sm_it=service_message_map.begin();sm_it!=service_message_map.end();sm_it++){
		std::string::size_type pos=sm_it->first.find('|');
		if(pos==std::string::npos)continue;

		XmlRpcValue params;
		params[0]=id;
		params[1]=sm_it->first.substr(0, pos);
		params[2]=sm_it->first.substr(pos+1);
		params[3]=sm_it->second;

		XmlRpcCallAsync("event", params, url);
		i++;
		LOG(Logger::LOG_DEBUG, "ServiceEvent %s", params.toText().c_str());
	}
	MulticallCollectEnd();
}

void HSSManager::GetServiceMessages(XmlRpc::XmlRpcValue* messages)
{
	int i=0;
	for(t_service_message_map::iterator sm_it=service_message_map.begin();sm_it!=service_message_map.end();sm_it++){
		std::string::size_type pos=sm_it->first.find('|');
		if(pos==std::string::npos)continue;

		XmlRpcValue& params=(*messages)[i];
		params[0]=sm_it->first.substr(0, pos);//Channel address
		params[1]=sm_it->first.substr(pos+1);//Value name
		params[2]=sm_it->second;//Value

		i++;
	}
}

void HSSManager::MulticallCollectEnd(){
	if(multicall)multicall--;
	if(!multicall){
//		LOG(Logger::LOG_DEBUG, "Doing collected multicalls");
		t_multicall_map::iterator it;
		for(it=multicall_map.begin();it!=multicall_map.end();it++){
			//LOG(Logger::LOG_DEBUG, "%s::system.multicall(%s)", it->first.c_str(), it->second.toText().c_str());
			XmlRpcCallAsync("system.multicall", it->second, it->first);
		}
		multicall_map.clear();
//		LOG(Logger::LOG_DEBUG, "Done with collected multicalls");
	}
}

void HSSManager::ValidateServiceMessages()
{
	for(t_service_message_map::iterator sm_it=service_message_map.begin();sm_it!=service_message_map.end();){
		std::string::size_type pos=sm_it->first.find('|');
		if(pos==std::string::npos){
			service_message_map.erase(sm_it++);
		}else{
			if(!GetInstance(sm_it->first.substr(0, pos))){
				service_message_map.erase(sm_it++);
			}else{
				sm_it++;
			}
		}
	}
	char buffer[8];
	snprintf(buffer, sizeof(buffer), "%zu", service_message_map.size());
	SendUDPInfo("SERVICE", buffer);
}

bool HSSManager::LoadXmlRpcHandlers()
{
	char buffer[256];
	std::ifstream file(xmlrpcHandlersFilepath.c_str());
	if(!file.good())return false;
	while(file.good() && !file.eof()){
        std::string url;
		std::string id;
		file.getline(buffer, sizeof(buffer));
		url=buffer;
		std::string::size_type pos=url.find('\t');
		if(pos==std::string::npos)continue;
        id=url.substr(pos+1);
		url=url.substr(0, pos);
		if(file.good() && !file.eof()){
			//xmlrpc_handlers[url]=new HSSXmlRpcEventDispatcher(url,id);
			//xmlrpc_handlers[url]->Start();
			PlatformInit(url, id, true);
			//SendServiceEvents(url, id);
		}
	}
	return file.good();
}

bool HSSManager::SaveXmlRpcHandlers()
{
	std::ofstream file(xmlrpcHandlersFilepath.c_str());
	if(!file.good()) {
		return false;
	}
	for(t_handler_map::iterator it=xmlrpc_handlers.begin();it!=xmlrpc_handlers.end();it++){
		if(removeUnreachableClients && (it->second->isClientUnreachable())) {
			continue;
		}
		else {
			file<<it->first.c_str()<<"\t"<<it->second->GetId().c_str()<<std::endl;
		}
    }
	return file.good();
}

bool HSSManager::Init(const char* config_filename)
{
    config_file.Clear();
    std::string absolute_filename=OSCompat::FixPath(config_filename);
    if(config_file.ReadFromFile(absolute_filename.c_str())<=0){
        LOG(Logger::LOG_ERROR, "Config file %s not found", absolute_filename.c_str());
        return false;
    }
    std::string metadata_dir=config_file.GetStringValue("Metadata Dir");
    if(metadata_dir.empty()){
        metadata_dir=config_file.GetStringValue("Device Files Dir");
    }
    if(!metadata_dir.empty()){
        metadata_store.SetDirectory(OSCompat::FixPath(metadata_dir).c_str());
    }

	config_file.SetCurrentSection("");
	if(config_file.GetStringValue("Remove Unreachable Clients", "true").compare("false") == 0) {
		removeUnreachableClients = false;
		LOG(Logger::LOG_DEBUG, "Removal of unreachable clients deactivated.");
	}
	
	//Read property for xmlrpc handlers filepath and initialize xmlrpcHandlersFilepath
	config_file.SetCurrentSection("");
	xmlrpcHandlersFilepath = config_file.GetStringValue("XmlRpcHandlersFile", "");
	if(xmlrpcHandlersFilepath.empty()) {//set to default name of not set
		xmlrpcHandlersFilepath.append("/var/");
		xmlrpcHandlersFilepath.append(task_id);
		xmlrpcHandlersFilepath.append(".handlers");
	}
    return true;
}

PropertyMap& HSSManager::GetConfigPropertyMap()
{
    return config_file;
}

bool HSSManager::MetadataSet(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue& value)
{
    return metadata_store.Set(object_id, data_id, value);
}

bool HSSManager::MetadataGet(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue* value)
{
    return metadata_store.Get(object_id, data_id, value);
}

bool HSSManager::MetadataGetAll(const char* object_id, XmlRpc::XmlRpcValue* value)
{
    return metadata_store.GetAll(object_id, value);
}

bool HSSManager::MetadataDelete(const char* object_id)
{
    return metadata_store.Delete(object_id);
}

bool HSSManager::MetadataGetVolatile(const char* data_id, XmlRpc::XmlRpcValue* value)
{
	return metadata_store.GetVolatile(data_id, value);
}

bool HSSManager::MetadataSetVolatile(const char* data_id, XmlRpc::XmlRpcValue& value)
{
	return metadata_store.SetVolatile(data_id, value);
}

bool HSSManager::MetadataDeleteVolatile(const char* data_id)
{
	return metadata_store.DeleteVolatile(data_id);
}

void HSSManager::Ping(XmlRpcValue& callerId)
{

	ReportEvent("CENTRAL", "PONG", callerId);
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MetadataStore.h"
#include <fstream>
#include <OSCompat.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#endif

MetadataStore::MetadataStore(void)
{
}

MetadataStore::~MetadataStore(void)
{
}

void MetadataStore::SetDirectory(const char* dir)
{
    directory=dir;
    if(directory.size() && directory[directory.size()-1]!=OSCompat::PATH_SEPARATOR){
        directory+=OSCompat::PATH_SEPARATOR;
    }
}

bool MetadataStore::Set(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue& value)
{
    XmlRpc::XmlRpcValue object_data;
    GetAll(object_id, &object_data);
    object_data[data_id]=value;
    return Save(object_id, object_data);
}

bool MetadataStore::Delete(const char* object_id, const char* data_id)
{
    XmlRpc::XmlRpcValue object_data;
    GetAll(object_id, &object_data);
    XmlRpc::XmlRpcValue::ValueStruct& value_struct=object_data;
    value_struct.erase(data_id);
    return Save(object_id, object_data);
}

bool MetadataStore::Delete(const char* object_id)
{
    bool success=false;
    if(directory.empty())return false;
    std::string pattern=FilenameFromId(object_id);
    OSCompat::DirectoryLister dir_lister(directory.c_str());
    std::vector<std::string> delete_list;
    while(true){
        std::string entry=dir_lister.NextEntry();
        if(entry.empty())break;
        if( (entry.size()>5) && (entry.substr(entry.size()-5)==".meta") ){
            bool match=false;

            for(unsigned int i=0;!match;i++){
				// did we compare all characters? -> match
                if(i==pattern.size() && i==entry.size()-5)match=true;
				// do we have a match until the wildcard at the end of object_id? -> match
                if( (object_id[i]=='*') && (object_id[i+1]=='\0') )match=true;
				// are we at the end of object_id or entry_id? -> mismatch
                if(i==pattern.size() || i==entry.size()-5)break;
				// do we have a mismatch on the current character? -> mismatch
                if(pattern[i] != entry[i])break;
            }
            if(match){
                delete_list.push_back(entry);
                success=true;
            }
        }
    }
    for(unsigned int i=0;i<delete_list.size();i++){
        if(unlink((directory+delete_list[i]).c_str()) != 0)success=false;
    }
    return success;
}

bool MetadataStore::Get(const char* object_id, const char* data_id, XmlRpc::XmlRpcValue* value)
{
    XmlRpc::XmlRpcValue object_data;
    GetAll(object_id, &object_data);
    if(object_data.getType() == XmlRpc::XmlRpcValue::TypeStruct){
        if(object_data.hasMember(data_id)){
            *value=object_data[data_id];
            return true;
        }
    }
    return false;
}

bool MetadataStore::GetAll(const char* object_id, XmlRpc::XmlRpcValue* value)
{
    std::string filename=FilenameFromId(object_id);
    std::ifstream is((directory+filename+".meta").c_str());
    if(!is.good())return false;
    char* buffer=new char[1024];
    std::string content;
    while(is.good() && !is.eof()){
        is.read(buffer, 1024);
        content.append(buffer, is.gcount());
    }
    delete[] buffer;
    if(content.size()){
        int offset=0;
        return value->fromStream(content, &offset);
    }
    return false;
}

bool MetadataStore::Save(const char* object_id, XmlRpc::XmlRpcValue& data)
{
    if(directory.empty())return false;
    std::string filename=FilenameFromId(object_id);
    std::ofstream os((directory+filename+".meta").c_str());
    if(!os.good())return false;
    std::string content=data.toStream();
    os.write(content.data(), content.size());
    return os.good();
}

std::string MetadataStore::FilenameFromId(const char* object_id)
{
    std::string s;
    s.resize(strlen(object_id));
    for(unsigned int i=0;object_id[i];i++){
        char c=object_id[i];
        if( (c>='0' && c<='9') || (c>='A' && c<='Z') || (c>='a' && c<='z') )s[i]=c;
        else s[i]='_';
    }
    return s;
}


bool MetadataStore::SetVolatile(const char* data_id, XmlRpc::XmlRpcValue& value)
{
	volatileData[data_id] = value;
	return true;
}

bool MetadataStore::GetVolatile(const char* data_id, XmlRpc::XmlRpcValue* value)
{
	if (volatileData.hasMember(data_id)) 
	{
		*value = volatileData[data_id];
		return true;
	}
	else 
	{
		return false;
	}
	
}

bool MetadataStore::DeleteVolatile(const char* data_id)
{
    XmlRpc::XmlRpcValue::ValueStruct& value_struct = volatileData;
    value_struct.erase(data_id);
		return true;
}

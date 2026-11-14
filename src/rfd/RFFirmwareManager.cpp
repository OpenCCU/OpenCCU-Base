/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFFirmwareManager.h"
#include <Logger.h>
#include <OSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <OSCompat.h>
#include <fstream>
#include <ctype.h>

static const char* MAPFILENAME="fwmap";//"firmwarefileMapRfDevices";

RFFirmwareManager::RFFirmwareManager(void)
: userFirmwarePath("/etc/config/firmware/")
{
}

RFFirmwareManager::~RFFirmwareManager(void)
{
	Free();
}

void RFFirmwareManager::Free()
{
}

void RFFirmwareManager::SetFirmwarePaths(const std::string& path, const std::string& userFirmwarePath)
{
	this->firmwarePath = path;
	if(!userFirmwarePath.empty()) {
		this->userFirmwarePath = userFirmwarePath;
	}
	if(!this->firmwarePath.empty())
	{
		OSCompat::FixPath(this->firmwarePath);
		this->firmwarePath.append(1, OSCompat::PATH_SEPARATOR);
	}
	if(!this->userFirmwarePath.empty())
	{
		OSCompat::FixPath(this->userFirmwarePath);
		this->userFirmwarePath.append(1, OSCompat::PATH_SEPARATOR);
	}
	InitFilenameMap();
	RefreshUserFirmwareMap();
}

const std::string &RFFirmwareManager::GetFirmwarePath()
{
	return this->firmwarePath;
}

std::string RFFirmwareManager::GetFirmwareVersion(const int typeNumber)
{
	int version = 0;
	t_filenameMap::iterator it = mergedFilenameMap.find(typeNumber);
	if(it == mergedFilenameMap.end())
	{
		return "";
	}
	version = it->second.getVersion();
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d.%d", (version>>8)&0xff, version&0xff);
	return buffer;
}

std::string RFFirmwareManager::GetFirmwareVersion(const int typeNumber, int *outVersion)
{
	int version = 0;
	t_filenameMap::iterator it = mergedFilenameMap.find(typeNumber);
	if(it == mergedFilenameMap.end())
	{
		return "";
	}
	version = it->second.getVersion();
	*outVersion = version;
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d.%d", (version>>8)&0xff, version&0xff);
	return buffer;
}

UpdateFile RFFirmwareManager::GetFirmware(const int typeNumber)
{
	UpdateFile updateFile;
	if(mergedFilenameMap.find(typeNumber) != mergedFilenameMap.end())
	{
		if(!updateFile.Read(mergedFilenameMap[typeNumber].getAbsoluteFilePath(),typeNumber)) {
			LOG(Logger::LOG_ERROR, "RFFirmwareManager::GetFirmware(): Error reading update file for type %d",typeNumber);
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "RFFirmwareManager::GetFirmware(): No update file for type %d",typeNumber);
	}
	return updateFile;
}

void RFFirmwareManager::InitFilenameMap()
{
	filenameMap.clear();
	//Read firmware file (fwmap)
    FILE* f;
	char buffer[256];
	int typeNumber = 0;
	f=fopen((OSCompat::FixPath(firmwarePath+MAPFILENAME)).c_str(), "r");
    
    if(!f){
		LOG(Logger::LOG_ERROR, "unable to open file %s", (firmwarePath+MAPFILENAME).c_str());
        return;
    }
    while(fgets(buffer, 256, f)){
		char* type=strtok(buffer, " \t\n\r");
		//skip comments and empty lines
		if(!type || *type=='#' || *type<' ' || *type =='H' || *type == 'A' || *type == 'C')continue;
		char* filename=strtok(NULL, " \t\n\r#");
		if(!filename || *filename<' ')continue;
		char* version=strtok(NULL, " \t\n\r#");
		if(!version || *version<' ')continue;
		if(strstr(filename, ".enc")==NULL && strstr(filename, ".eq3")==NULL)continue;
		typeNumber = atoi(type);
		std::string filePath(firmwarePath);
		filePath.append(filename);
		filenameMap[typeNumber]=FirmwareFile(filePath.c_str(), version);
		LOG(Logger::LOG_DEBUG, "%s=%s(%s)", type, filename, version);
	}
	fclose(f);
	if(Logger::WouldLog(Logger::LOG_ALL)) {

		for(std::map<int,FirmwareFile>::iterator it = this->filenameMap.begin() ; it != this->filenameMap.end(); ++it) {
			int versionMajor = (it->second.getVersion()>>8)&0xff;
			int versionMinor =	it->second.getVersion()&0xff;
			LOG(Logger::LOG_DEBUG, "RFFirmwareManager: Registered device firmware version %d.%d for device id %d.", versionMajor, versionMinor, it->first);
		}

	}
}

void RFFirmwareManager::RefreshUserFirmwareMap() {
	mergedFilenameMap = filenameMap;
	std::vector<std::pair<int,FirmwareFile> > userFirmwareFiles;
	ReadUserFirmwarePath(userFirmwareFiles);
	//Merge user firmware files into fwmap firmware files
	for(unsigned int i = 0; i < userFirmwareFiles.size(); i++) {
		std::pair<int,FirmwareFile> userFirmwareFile = userFirmwareFiles.at(i);
		std::map<int,FirmwareFile>::iterator it = mergedFilenameMap.find(userFirmwareFile.first);

		if(it != mergedFilenameMap.end()) {
			if(userFirmwareFile.second.getVersion() > it->second.getVersion()) {
				if(Logger::WouldLog(Logger::LOG_DEBUG)) {
						int versionMajor = (userFirmwareFile.second.getVersion()>>8)&0xff;
						int versionMinor =	userFirmwareFile.second.getVersion()&0xff;
						LOG(Logger::LOG_ALL, "RFFirmwareManager: Registered user-deployed device firmware version %d.%d for device id %d.", versionMajor, versionMinor, userFirmwareFile.first);
				}
				mergedFilenameMap[userFirmwareFile.first] = userFirmwareFile.second;
			}
			else {
				if(Logger::WouldLog(Logger::LOG_DEBUG)) {
					int versionMajor = (userFirmwareFile.second.getVersion()>>8)&0xff;
					int versionMinor =	userFirmwareFile.second.getVersion()&0xff;
					LOG(Logger::LOG_ALL, "RFFirmwareManager: Ignoring user-deployed device firmware version %d.%d for device id %d. (Higher version available.)", versionMajor, versionMinor, userFirmwareFile.first);
				}
			}
		}
		else {
			if(Logger::WouldLog(Logger::LOG_DEBUG)) {
				int versionMajor = (userFirmwareFile.second.getVersion()>>8)&0xff;
				int versionMinor =	userFirmwareFile.second.getVersion()&0xff;
				LOG(Logger::LOG_ALL, "RFFirmwareManager: Registered user-deployed device firmware version %d.%d for device id %d.", versionMajor, versionMinor, userFirmwareFile.first);
			}
			mergedFilenameMap.insert(userFirmwareFile);
		}
	}
}

void RFFirmwareManager::ReadUserFirmwarePath(std::vector< std::pair<int, FirmwareFile> >& userFirmwareFiles)
{
	userFirmwareFiles.clear();
	if(!userFirmwarePath.empty()) {
		OSCompat::DirectoryLister directoryLister(userFirmwarePath.c_str(), OSCompat::DirectoryLister::FLAG_LIST_DIRS);
		std::string nextDir = directoryLister.NextEntry();
		while(!nextDir.empty()) {
			if(nextDir.size() <= 2) {
				if(nextDir.compare("..") == 0 || nextDir.compare("../") == 0 || nextDir.compare(".") == 0 || nextDir.compare("./") == 0) {
					nextDir = directoryLister.NextEntry();
					continue;
				}
			}
			std::string currentPath(userFirmwarePath);
			currentPath.append(nextDir);
			std::string infoFileName(currentPath);
			infoFileName.append(1, OSCompat::PATH_SEPARATOR);
			infoFileName.append("info");
			std::string firmwareFileName;
			std::string version;
			int typeCode;
			if(ParseInfoFile(infoFileName, typeCode, version))
			{
				OSCompat::DirectoryLister directoryLister(currentPath.c_str(), OSCompat::DirectoryLister::FLAG_LIST_FILES);
				std::string fileEntry = directoryLister.NextEntry();
				while(!fileEntry.empty()) {
					if((fileEntry.size() >= 4) && (fileEntry.find(".eq3") != std::string::npos ) && (fileEntry.rfind(".eq3") == (fileEntry.size()-4))  ) {
						firmwareFileName.append(currentPath);
						firmwareFileName.append(1, OSCompat::PATH_SEPARATOR);
						firmwareFileName.append(fileEntry);
						break;
					}
					fileEntry = directoryLister.NextEntry();
				}
				if(!firmwareFileName.empty()) {
					FirmwareFile fwFile(firmwareFileName.c_str(), version.c_str());
					userFirmwareFiles.push_back(std::make_pair(typeCode, fwFile));
				}
			}
			nextDir = directoryLister.NextEntry();
		}
	}
}

bool RFFirmwareManager::ParseInfoFile(const std::string& infoFilePath, int& typeCode, std::string& version)
{
	std::ifstream iStream;
	iStream.open(infoFilePath.c_str(), std::ifstream::in);
	if(iStream.good()) {
		std::string line;
		bool gotVersion = false;
		bool gotTypeCode = false;
		while(std::getline(iStream, line)) {

			TrimLine(line);
			if(line.empty()) {
				continue;
			}
			else if(line.at(0) == '#') {
				continue;
			}
			else {
				std::string::size_type assCharIndex = line.find('=');
				if(assCharIndex == std::string::npos) {
					continue;
				}
				else {
					std::string key = line.substr(0, assCharIndex);
					TrimLine(key);
					if(line.size() >  (assCharIndex+1)) {
						std::string value = line.substr(assCharIndex+1);
						TrimLine(value);

						if(key.compare("TypeCode") == 0) {
							typeCode = atoi(value.c_str());
							gotTypeCode = true;
						}
						else if(key.compare("FirmwareVersion") == 0) {
							version = value;
							gotVersion = true;
						}
						else {
							continue;
						}
					}
				}
			}
		}
		if(gotTypeCode && gotVersion) {
			return true;
		}
		iStream.close();
	}
	return false;
}

void RFFirmwareManager::TrimLine(std::string& line) {
	int whiteCnt = 0;
	for(unsigned int i = 0; i < line.size(); i++) {//find index of first non-space character
		if( ! isspace(static_cast<int>(line.at(i)))) {
			whiteCnt = i;
			break;
		}
	}
	if(whiteCnt > 0) {
		line.assign(line.substr(whiteCnt));
	}
	whiteCnt = 0;
	for(unsigned int i = line.size() - 1; i >= 0; i--) {
		if(isspace(static_cast<int>(line.at(i)))) {
			whiteCnt++;
		}
		else {
			break;
		}
	}
	if(whiteCnt > 0) {
		line.assign(line.substr(0, (line.size()-whiteCnt)));
	}
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <Logger.h>
#include <SyslogLogger.h>
#include <stdlib.h>
#include <stdio.h>
#include <PropertyMap.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/wait.h>
#include <xmlParser.h>
#include <fstream>
#include <iostream>



//Declarations
/**\brief Prints usage of this application.*/
void printUsage();

/**\brief Parses hs485d.conf and extract interface id of HMWLGW.*/
bool parseHS485dConf(const std::string& path, std::string& interfaceId);

/**\brief Starts hs485d.*/
int startApp(const std::string& path, const std::string& interfaceId, std::string logLevel);//logLevel from /etc/config/syslog

/**\brief Updates InterfacesList.xml.*/
bool updateInterfacesXML(const std::string& path, const bool wiredInterfaceAvailable);

/** \brief Writes data to file.*/
bool writeToInterfacesXMLFile(const std::string& interfacesXMLFilePath, const char* data);

/** \brief Retrieves configured xmlrpc bind port (as string). Defaults to 2000.*/
std::string retrieveXmlRpcPort();

/** \brief Returns trimmed copy str.*/
std::string trim(const std::string& str);

//Implementations
int main(int argc, char** argv)
{
	int logLevel = 5;
	std::string logLevelStr;
	std::string path;
	bool startHS485d = true;
	bool writeInterfacesXML = true;
	bool startAsDaemon = true;
	std::string interfacesXmlPath("/etc/config/InterfacesList.xml");
	//Parse args
	if(argc < 2) {
		printUsage();
		return 1;
	}
	else {
		for(int i = 1; i < argc; i++) {
			std::string s = argv[i];
			if(s.compare("-l") == 0) {
				i++;
				if(i < argc) {
					logLevel = atoi(argv[i]);
					logLevelStr = argv[i];
				}
				else {
					printUsage();
					return 1;
				}
			}
			else if(s.compare("-ds") == 0) {
				startHS485d = false;
			}
			else if(s.compare("-dw") == 0) {
				writeInterfacesXML = false;
			}
			else if(s.compare("-dd") == 0) {
				startAsDaemon = false;
			}
			else if(s.compare("-ilp") == 0) {
				if(i < argc) {
					i++;
					interfacesXmlPath = argv[i];
				}
				else {
					printUsage();
					return 1;
				}
			}
			else {
				path = argv[i];
			}			
		}
	}
	if(path.empty()) {
		printUsage();
	}

	if(startAsDaemon) {
		if(daemon(0,0)) {
			LOG(Logger::LOG_FATAL_ERROR, "Could not daemonize process");	
		}
	}
	
	SyslogLogger* log = new SyslogLogger();
	log->SetLevel((Logger::LogLevel)logLevel);

	if(path.empty()) {
		LOG(Logger::LOG_FATAL_ERROR, "Path to hs485d config file not given -> Exiting.");
	}

	std::string interfaceId;
	bool done = parseHS485dConf(path, interfaceId);
	if(done) {
		if(writeInterfacesXML) {
			updateInterfacesXML(interfacesXmlPath, (!interfaceId.empty()) );
		}
		if(startHS485d) {
			return startApp(path, interfaceId, logLevelStr);
		}
		return 0;
	}
	else {
		updateInterfacesXML(interfacesXmlPath, false);
	}

	return 0;
}

int startApp(const std::string& path, const std::string& interfaceId, std::string logLevel) {
	if(interfaceId.length() == 1) {
		int retVal = 0;
		while(retVal != EACCES && retVal != ENOEXEC) {	
			pid_t childPID = fork();
			retVal = 0;
			if(childPID == 0) {
				retVal = execl("/bin/hs485d", "/bin/hs485d", "-l", logLevel.c_str(), "-g", "-i", interfaceId.c_str(), "-f", path.c_str(), NULL);
				exit(0);
			}
			else if(childPID < 0){
				LOG( Logger::LOG_ERROR, "Error creating process.");
			}
			else {
				//parent
				//write pid file of hs485Loader
				pid_t loaderPID = getpid();
				FILE* pFile = fopen("/var/run/hs485dLoader.pid", "wb");
				if(pFile != NULL) {
					fprintf(pFile, "%d", loaderPID);
					fflush(pFile);
					fclose(pFile);
				}
				else {
					LOG(Logger::LOG_ERROR, "Cannot write pid file of hs485dLoader");
				}
				int status;
				wait(&status);
			}
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "Something is wrong with interfaceId");
	}
    LOG(Logger::LOG_DEBUG, "hs485dLoader dies.");
    return 0;
}

bool parseHS485dConf(const std::string& path, std::string& interfaceId)
{
	PropertyMap configData;
	if(path.empty()) {
		return false;
	}
	if( (configData.ReadFromFile(path) < 0) ) {//try to read file
		//if it failed -> try to restore .bak file if there is one
		std::string bakPath(path.substr(0, path.length()-3));
		bakPath.append(".xml");
		rename(bakPath.c_str(), path.c_str());
		if( (configData.ReadFromFile(path) < 0) ) {
				return false;
		}
	}

	interfaceId.clear();
    PropertyMap::StringList sections=configData.ListSections();
    for(PropertyMap::StringList::iterator it=sections.begin();it!=sections.end();it++)
    {
        std::string& section=*it;
        if(section.find("Interface ")==0)
        {
            configData.SetCurrentSection(section);
            std::string interfaceNr = configData.GetCurrentSection();
            interfaceNr = interfaceNr.substr(10, 1);
            std::string type=configData.GetStringValue("Type", "");
            if(type.compare("HMWLGW") == 0) {
            	interfaceId = interfaceNr;
            	return true;
            }
            else {
                LOG( Logger::LOG_ERROR, "Interface type %s not supported", type.c_str());
                break;//continue;
            }
        }
    }
    return false;
}


bool updateInterfacesXML(const std::string& path, const bool wiredInterfaceAvailable)
{
	std::string port = retrieveXmlRpcPort();
	const std::string ifListFilePath = path;
	std::ifstream xmlfileStream;
	xmlfileStream.open(ifListFilePath.c_str(), std::ifstream::in);
	std::string xmlstring;
	char* buffer = new char[1024];
	std::streamsize readChars = 0;
	do {
		readChars = xmlfileStream.readsome(buffer, 1024);
		xmlstring.append(buffer, readChars);
	} while(readChars > 0);
	delete[] buffer;
	xmlfileStream.close();

	std::string::size_type nameTagBeginIndex = xmlstring.find("<name>BidCos-Wired</name>");
	if(wiredInterfaceAvailable) {//Available
		//if(hmwlgwIPCNodeIndex == -1) {// but not defined in InterfaceList.xml

		if(nameTagBeginIndex == std::string::npos) {
			//-> Add Entry
			//find closing interfaces tag
			std::string::size_type index = xmlstring.find_last_of("</interfaces>");
			if(index != std::string::npos) {
				std::string wiredEntry("\n\t<ipc>\n\t\t<name>BidCos-Wired</name>\n\t\t<url>xmlrpc_bin://127.0.0.1:");
						wiredEntry.append(port);
						wiredEntry.append("</url>\n\t\t<info>BidCoS-Wired</info>\n\t</ipc>");
				index -= 13;
				xmlstring.insert(index, wiredEntry);
			}
			writeToInterfacesXMLFile(path, xmlstring.c_str());
			//free(xmlstringArray);
		}
	}
	else {//Not available
		if(nameTagBeginIndex != std::string::npos) { //but defined in InterfaceList.xml
			std::string::size_type indexStart = xmlstring.rfind("<ipc>", nameTagBeginIndex);
			if(indexStart != std::string::npos) {
				//remove leading white space
				while( (indexStart-1 > 0) && ((xmlstring.at(indexStart-1) == ' ') || (xmlstring.at(indexStart-1) == '\t') || (xmlstring.at(indexStart-1) == '\n')) ) {
					indexStart--;
				}

				std::string::size_type indexEnd = xmlstring.find("</ipc>", nameTagBeginIndex);
				indexEnd += 5;
				std::string resultStr(xmlstring.substr(0, indexStart));
				resultStr.append(xmlstring.substr(indexEnd+1));
				writeToInterfacesXMLFile(path, resultStr.c_str());
			}
			else {
				return false;
			}
		}
	}
	return true;
}


bool writeToInterfacesXMLFile(const std::string& interfacesXMLFilePath, const char* data)
{
	const std::string newFilePath(interfacesXMLFilePath+".new");
	const std::string bakFilePath(interfacesXMLFilePath+".bak");
	const std::string xmlFilePath(interfacesXMLFilePath);

	//Write data to newFile
	FILE* fd = fopen(newFilePath.c_str(), "wb");
	if(fd == NULL) {
		unlink(newFilePath.c_str());
		return false;
	}
	fprintf(fd, "%s", data);
	fflush(fd);
#ifndef WIN32
	fsync(fileno(fd));
#endif
	fclose(fd);

	unlink(bakFilePath.c_str());//remove old bak file if it exists
	rename(xmlFilePath.c_str(), bakFilePath.c_str());//rename current file to bak file
	if(rename(newFilePath.c_str(), xmlFilePath.c_str())) {
		return false;
	}
	unlink(bakFilePath.c_str());
	return true;
}

std::string retrieveXmlRpcPort() 
{
	char* buffer = new char[256];
	std::ifstream ifs;
	ifs.open("/etc/hs485d.port", std::ifstream::in);
	while(ifs.good()) {
		ifs.getline(buffer, 256);
		std::string s(buffer);
		s = trim(s);
		std::string base("01234567890");
		if(!s.empty() && (s.find_first_not_of(base) == std::string::npos)) {
			delete[] buffer;
			return s;
		}
	}
	delete[] buffer;
	return std::string("2000");
}

std::string trim(const std::string& str)
{
    std::string::size_type first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    std::string::size_type last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void printUsage()
{
	printf("hs485dLoader -l LEVEL [-ds] [-dw] [-dd] [-ilf /path/to/InterfacesList.xml] /pathto/hs485d.conf\n\n");
	printf("\t-l LEVEL:  Log Level for hs485d; Default is 5\n");
	printf("\t-ds:       Do not start hs485d, just write InterfacesList.xml\n");
	printf("\t-dw:       Do not write /etc/config/InterfacesList.xml, just start hs485d\n");
	printf("\t-dd:       Do not start as daemon process.\n");
	printf("\t-ilp:      Path to InterfacesList.xml (default is /etc/config/InterfacesList.xml)\n");
}

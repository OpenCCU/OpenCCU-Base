// SetInterfaceClock.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include <ctime>
#include <stdio.h>
#include <XmlRpc.h>
#include <TimeZoneInfo.h>
#include <SyslogLogger.h>
#include <fstream>
#include <iostream>


void printUsage();
std::string trim(const std::string& str);
std::string readPortFromFile(const char* filename);

using namespace XmlRpc;

int main(int argc, char* argv[])
{
	Logger* log = (Logger*)new SyslogLogger("SetInterfaceClock");
	log->SetLevel((Logger::LogLevel)1);

	//Check program argument
	std::string url;
	if(argc == 2) {
		url = argv[1];
	}
	else if(argc == 1) {
		std::string port = readPortFromFile("/etc/rfd.port");
		if(port.empty()) {
			port = "2001";
		}
		url = "127.0.0.1:" + port;
	}
	else {
		LOG(Logger::LOG_ERROR, "setInterfaceClock - Wrong usage");
		printUsage();
		return 1;
	}

	//Create xmlrpc client 
	XmlRpcClient client(url);
	XmlRpcValue params;
	XmlRpcValue result;
	//Get time information
	TimeZoneInfo tzInfo;
	time_t utcOffsetMinutes = tzInfo.GetUTCOffset() / 60;
	time_t utcSeconds = time(NULL);
	//Add params
	params[0] = (int)utcSeconds;
	params[1] = (int)utcOffsetMinutes;
	//xmlrpc-call
	bool success = client.execute("setInterfaceClock", params, result);
	if ((success) && (client.isFault() == false))
	{
		LOG(Logger::LOG_INFO, "setInterfaceClock - Set interface time to utcSeconds=%d utcOffsetMinutes=%d", utcSeconds, utcOffsetMinutes);
		return 0;
	}
	else
	{
		LOG(Logger::LOG_ERROR, "setInterfaceClock - Error setting interface time: %s", result.toText().c_str());
		return 2;
	}
}

std::string trim(const std::string& str)
{
    unsigned int first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    unsigned int last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string readPortFromFile(const char* filename) {
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    char* buffer = new char[256];
    std::string base("01234567890");
    while(ifs.good()) {
        ifs.getline(buffer, 256);
        std::string port(buffer);
        port = trim(port);
        if(!port.empty() && (port.find_first_not_of(base) == std::string::npos)) {
            delete[] buffer;
            return port;
        }
    }
    delete[] buffer;
    return std::string("");
}

void printUsage() {
	std::string msg;
	msg.append("\nSetInterfaceClock [IP:Port]\n");
	msg.append("- Calls xml-rpc method SetInterfaceClock. -\n");
	msg.append("Paremeters:\n");
	msg.append("IP:		IP address (optional)\n");
	msg.append("Port:	Port number (optional)\n");
	msg.append("If called without paremeters, the IP address defaults to 127.0.0.1.\nThe port is read from /etc/rfd.port, fallback value is still 2001.\n");
	std::cout << msg;
}

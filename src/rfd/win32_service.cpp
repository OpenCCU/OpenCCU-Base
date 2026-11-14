/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */

#include <windows.h>
#include <WinSvc.h>
#include <Logger.h>
#include <TimerQueue.h>
#include <TimerTarget.h>
#include "win32_service.h"
#include <XmlRpc.h>

static SERVICE_STATUS		ss; 
static SERVICE_STATUS_HANDLE	hStatus; 
static SERVICE_DESCRIPTION	service_descr = {"BidCoS-Service"};
static const char SERVICE_NAME[] = "BidCoS-Service";

extern XmlRpc::XmlRpcServer s;

bool win32_service_install()
{
	SC_HANDLE	hSCM, hService;
	char		cmdline[MAX_PATH+8];


    if ((hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL){
        LOG(Logger::LOG_ERROR, "Error opening SCM (%d)", GetLastError());
        return false;
    }

    GetModuleFileName(NULL, cmdline, MAX_PATH);

    strcat(cmdline, " -d");

	hService = CreateService(hSCM, SERVICE_NAME, SERVICE_NAME,
	    SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
	    SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, cmdline,
	    NULL, NULL, NULL, NULL, NULL);

    if (!hService){
        LOG(Logger::LOG_ERROR, "Error installing service (%d)", GetLastError());
        return false;
    }
	ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &service_descr);
    LOG(Logger::LOG_INFO, "Service successfully installed");
    return true;
}

bool win32_service_uninstall()
{
	SC_HANDLE	hSCM, hService;

    if ((hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL){
        LOG(Logger::LOG_ERROR, "Error opening SCM (%d)", GetLastError());
        return false;
    }
	if ((hService = OpenService(hSCM, SERVICE_NAME, DELETE)) == NULL) {
        LOG(Logger::LOG_ERROR, "Error opening service (%d)", GetLastError());
        return false;
    }
	if (!DeleteService(hService)) {
        LOG(Logger::LOG_ERROR, "Error deleting service (%d)", GetLastError());
        return false;
    }
    LOG(Logger::LOG_INFO, "Service successfully uninstalled");
    return true;
}

static void WINAPI ControlHandler(DWORD code) 
{ 
	if (code == SERVICE_CONTROL_STOP || code == SERVICE_CONTROL_SHUTDOWN) {
		ss.dwWin32ExitCode	= 0; 
		ss.dwCurrentState	= SERVICE_STOPPED; 
	} 
 
	SetServiceStatus(hStatus, &ss);
}

static void WINAPI ServiceMain(int argc, char *argv[]) 
{
	char	path[MAX_PATH], *av[] = {"bidcos_service", path, NULL};

	ss.dwServiceType      = SERVICE_WIN32; 
	ss.dwCurrentState     = SERVICE_RUNNING; 
	ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	hStatus = RegisterServiceCtrlHandler(SERVICE_NAME, ControlHandler);
	SetServiceStatus(hStatus, &ss); 

    while (ss.dwCurrentState == SERVICE_RUNNING){
		int64_t timeout=TimerTarget::s_timerQueue.TimeBeforeNextDue();
		while(!timeout){
			TimerTarget::s_timerQueue.Execute();
			timeout=TimerTarget::s_timerQueue.TimeBeforeNextDue();
		}
        if(timeout>2000)timeout=2000;
		s.work(timeout);
	}

	ss.dwCurrentState  = SERVICE_STOPPED; 
	ss.dwWin32ExitCode = -1; 
	SetServiceStatus(hStatus, &ss); 
}

bool try_to_run_as_nt_service(void)
{
	static SERVICE_TABLE_ENTRY service_table[] = {
		{(char*)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(service_table))return true;
    return false;
}

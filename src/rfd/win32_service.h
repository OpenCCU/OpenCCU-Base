/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _WIN32_SERVICE_H_
#define _WIN32_SERVICE_H_
#ifdef WIN32
bool win32_service_install();
bool win32_service_uninstall();
bool try_to_run_as_nt_service(void);
#endif
#endif

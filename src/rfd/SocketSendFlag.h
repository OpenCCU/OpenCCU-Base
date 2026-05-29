/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _SOCKET_SEND_FLAG_H_
#define _SOCKET_SEND_FLAG_H_

#ifndef WIN32
# define SOCKET_SEND_FLAGS MSG_NOSIGNAL
#else
# define SOCKET_SEND_FLAGS 0
#endif

#endif
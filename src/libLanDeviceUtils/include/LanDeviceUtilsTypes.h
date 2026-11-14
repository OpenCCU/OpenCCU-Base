/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_LANDEVICEUTILSTYPES_H_
#define _LIBLANDEVICEUTILS_LANDEVICEUTILSTYPES_H_

namespace LDU {

	enum ProtocolEnum {
		PROTOCOL_UNKNOWN = 0,
		PROTOCOL_EQ3LANIFCFG = 1,
		PROTOCOL_EQ3CONFIG = 2
	};

	enum RoutingSchemeEnum {
		ROUTINGSCHEME_UNICAST = 1,
		ROUTINGSCHEME_MULTICAST = 2,
		ROUTINGSCHEME_BROADCAST = 4,
	};
}
#endif
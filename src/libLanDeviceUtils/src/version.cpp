// Version of libLanDeviceUtils (see version.sh and CHANGELOG.md). A program can print it
// by declaring: extern "C" const char* landeviceutils_version();
#include "generated-LanDeviceUtils-version.h"

extern "C" const char* landeviceutils_version()
{
	return LANDEVICEUTILS_VERSION;
}

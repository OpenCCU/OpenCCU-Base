// Version of libLanDeviceUtils (see version.sh and CHANGELOG.md).
// Declared in landeviceutils_version.h.
#include "landeviceutils_version.h"
#include "generated-LanDeviceUtils-version.h"

extern "C" const char* landeviceutils_version()
{
	return LANDEVICEUTILS_VERSION;
}

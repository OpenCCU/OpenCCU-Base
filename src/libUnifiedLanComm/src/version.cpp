// Version of libUnifiedLanComm (see version.sh and CHANGELOG.md).
// Declared in unifiedlancomm_version.h.
#include "unifiedlancomm_version.h"
#include "generated-UnifiedLanComm-version.h"

extern "C" const char* unifiedlancomm_version()
{
	return UNIFIEDLANCOMM_VERSION;
}

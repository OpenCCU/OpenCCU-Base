// Version of libUnifiedLanComm (see version.sh and CHANGELOG.md). A program can print it
// by declaring: extern "C" const char* unifiedlancomm_version();
#include "generated-UnifiedLanComm-version.h"

extern "C" const char* unifiedlancomm_version()
{
	return UNIFIEDLANCOMM_VERSION;
}

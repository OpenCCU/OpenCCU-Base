// Version of libhsscomm (see version.sh and CHANGELOG.md).
// Declared in hsscomm_version.h.
#include "hsscomm_version.h"
#include "generated-hsscomm-version.h"

extern "C" const char* hsscomm_version()
{
	return HSSCOMM_VERSION;
}

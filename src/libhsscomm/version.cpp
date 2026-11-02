// Version of libhsscomm (see version.sh and CHANGELOG.md). A program can print it
// by declaring: extern "C" const char* hsscomm_version();
#include "generated-hsscomm-version.h"

extern "C" const char* hsscomm_version()
{
	return HSSCOMM_VERSION;
}

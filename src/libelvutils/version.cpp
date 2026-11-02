// Version of libelvutils (see version.sh and CHANGELOG.md). A program can print it
// by declaring: extern "C" const char* elvutils_version();
#include "generated-elvutils-version.h"

extern "C" const char* elvutils_version()
{
	return ELVUTILS_VERSION;
}

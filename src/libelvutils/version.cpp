// Version of libelvutils (see version.sh and CHANGELOG.md).
// Declared in elvutils_version.h.
#include "elvutils_version.h"
#include "generated-elvutils-version.h"

extern "C" const char* elvutils_version()
{
	return ELVUTILS_VERSION;
}

// Version of libeq3config (see version.sh and CHANGELOG.md).
// Declared in eq3config_version.h.
#include "eq3config_version.h"
#include "generated-eq3config-version.h"

extern "C" const char* eq3config_version()
{
	return EQ3CONFIG_VERSION;
}

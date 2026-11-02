// Version of libeq3config (see version.sh and CHANGELOG.md). A program can print it
// by declaring: extern "C" const char* eq3config_version();
#include "generated-eq3config-version.h"

extern "C" const char* eq3config_version()
{
	return EQ3CONFIG_VERSION;
}

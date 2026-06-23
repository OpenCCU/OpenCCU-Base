#include <HelpSystem.h>
static HelpSystem helpSystemInstance;

#include <netconfigcmd.h>
static NetConfigCmd NetConfigCmdInstance;

#include <update-lgw-firmware.h>
static LGWFirmwareUpdate LGWFirmwareUpdateInstance;

#include <update-coprocessor.h>
static CoprocessorUpdate CoprocessorUpdateInstance;

#include <setlgwkey.h>
static SetLGWKey SetLGWKeyInstance;

#include <rfd-interface-copy.h>
static RfdInterfacCopy RfdInterfacCopyInstance;

#include <ReadDefaultRFAddress.h>
static ReadDefaultRFAddress ReadDefaultRFAddressInstance;

#include <wait-for-file.h>
static WaitForFile WaitForFileInstance;

#include <test-coprocessor.h>
static CoprocessorTest CoprocessorTestInstance;


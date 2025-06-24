# Makefile for linedriver

# Load PROJECT_DIR and PLATFORM from local configuration file
LOCAL_CONFIG_FILE=config.mk
ifneq ($(wildcard $(LOCAL_CONFIG_FILE)),)
include $(LOCAL_CONFIG_FILE)
CONFIGURE=
else
CONFIGURE=configure
endif

# Is PROJECT_DIR specified?
ifeq ($(PROJECT_DIR),)
$(error "missing PROJECT_DIR, run make PROJECT_DIR=<project dir>")
endif

# Is PLATFORM specified?
ifeq ($(PLATFORM),)
$(error "missing PLATFORM, run make PLATFORM=<platform>")
endif

# Load project configuration file
PROJECT_CONFIG_FILE = $(PROJECT_DIR)/platform/$(PLATFORM).mk
ifneq ($(wildcard $(PROJECT_CONFIG_FILE)),)
include $(PROJECT_CONFIG_FILE)
else
$(error "missing project configuration; is PLATFORM=$(PLATFORM) valid?")
endif

ROMFSINST = $(shell if [ -x ../helper_scripts/romfs-inst.sh ]; then echo ../helper_scripts/romfs-inst.sh; else echo /usr/bin/romfs-inst.sh; fi)

TARGET = hs485d

LIBELVUTILSPATH = ../elvutils
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

LIBHSSCOMMPATH = ../libhsscomm
#LIBHSSCOMMPATH = $(LIBHSSCOMM_DIR)
LIBHSSCOMM = hsscomm

LIBXMLPARSERPATH = ../xmlparser
#LIBXMLPARSERPATH = $(XMLPARSER_DIR)
LIBXMLPARSER = xmlparser

XMLRPCPATH = ../xmlrpc
#XMLRPCPATH = $(XMLRPC_DIR)
XMLRPCLIB = libXmlRpc.so
XMLRPCINCLUDE = $(XMLRPCPATH)/src

LIBUNIFIEDLANCOMM_PATH = ../libUnifiedLanComm
#LIBUNIFIEDLANCOMM_PATH = $(LIBUNIFIEDLANCOMM_DIR)
LIBUNIFIEDLANCOMM = UnifiedLanComm
LIBUNIFIEDLANCOMM_INCLUDE = $(LIBUNIFIEDLANCOMM_PATH)/include

LIBLANDEVICEUTILS_PATH = ../libLanDeviceUtils
#LIBLANDEVICEUTILS_PATH = $(LIBLANDEVICEUTILS_DIR)
LIBLANDEVICEUTILS = LanDeviceUtils
LIBLANDEVICEUTILS_INCLUDE = $(LIBLANDEVICEUTILS_PATH)/include

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0
else
OPTIMIZE = -O2 -g
endif

ifeq ($(PROJECT_VERSION),)
PROJECT_VERSION = $(shell ./version.sh)
endif

TARGETOBJS = $(TARGET).o \
	HS485Central.o \
	HS485Channel.o \
	HS485ChannelDescription.o \
	HS485DeviceDescription.o \
	HS485Device.o \
	HS485EEpromCache.o \
	HS485FirmwareManager.o \
	HS485Frame.o \
	HS485CommMessage.o \
	HS485CommMessageCCU1.o \
	HS485CommMessageDecoder.o \
	HS485Controller.o \
	HS485ControllerCCU1.o \
	HS485LogicalInstance.o \
	HS485LogicalTypeAddress.o \
	HS485MaintenanceChannel.o \
	HS485Manager.o \
	HS485Paramset.o \
	HS485PhysicalAddress.o \
	HS485PhysicalDataInterfaceCentralCommand.o \
	HS485PhysicalDataInterfaceCommand.o \
	HS485PhysicalDataInterfaceEEprom.o \
	HS485PhysicalDataInterfaceStore.o \
	HS485SystemDescription.o \
	HS485TypeConversionAddressArray.o \
	XmlRpcMethods.o \
	LGWPortWrapper.o \
	HS485CommMessageLGW.o \
	HS485ControllerLGW.o \
	HMWLGWTransportFrame.o \
	HMWLGWCommand.o

CC = $(CROSS)g++
CXX = $(CROSS)g++
STRIP = $(CROSS)strip

CXX_FLAGS +=  $(OPTIMIZE) -I./ -I$(LIBELVUTILSPATH) -I$(LIBHSSCOMMPATH) -I$(LIBXMLPARSERPATH) -I$(XMLRPCINCLUDE) -I$(LIBUNIFIEDLANCOMM_INCLUDE) -I$(LIBLANDEVICEUTILS_INCLUDE) -Wall -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64 $(MACHINE_OPTION)
LD_FLAGS += $(MACHINE_OPTION) -lpthread -L$(LIBUNIFIEDLANCOMM_PATH) -l$(LIBUNIFIEDLANCOMM) -L$(LIBLANDEVICEUTILS_PATH) -l$(LIBLANDEVICEUTILS) -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) -L$(LIBHSSCOMMPATH) -l$(LIBHSSCOMM) -L$(LIBXMLPARSERPATH) -l$(LIBXMLPARSER) -L$(XMLRPCPATH) -lXmlRpc -lc -lm

all: $(CONFIGURE) info prebuild $(TARGET)

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

xmlrpc_clean:
	make CROSS=$(CROSS) -C $(XMLRPCPATH) clean

xmlrpc:
	make CROSS=$(CROSS) -C $(XMLRPCPATH) $(XMLRPCLIB)

xmlrpc_install:
	make CROSS=$(CROSS) -C $(XMLRPCPATH) install

info:
	@echo ""
	@echo "**************************"
	@echo "* MODULE $(TARGET)"
	@echo "**************************"
	@echo "* PROJECT_DIR = $(PROJECT_DIR)"
	@echo "* TARGET_DIR  = $(TARGET_DIR)"
	@echo "* BIN_DIR     = $(BIN_DIR)"
	@echo "**************************"
	@echo ""

prebuild:
	echo "#define HS485D_VERSION \"$(PROJECT_VERSION)\"" > generated-hs485d-version.h

$(TARGET): $(TARGETOBJS)
	$(CXX) -o $(TARGET) $(TARGETOBJS) $(LD_FLAGS)
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET)


%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

install: $(TARGET)
	$(ROMFSINST) /bin/$(TARGET)


clean:
	rm -rf *.o
	rm -rf config.mk
	rm -rf $(TARGET)
	rm -f $(TARGET).debug


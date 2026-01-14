#Makefile for libLanDeviceUtils library (.so)

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

#The target of this Makefile
TARGET=libLanDeviceUtils.so


#enable/disable debug stuff
ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0
STRIP=true
else
OPTIMIZE = -O2 -g
STRIP=$(CROSS)strip
endif

#Dependencies
BIDCOSUTILSPATH = ../elvutils
#BIDCOSUTILSPATH = $(ELVUTILS_DIR)
BIDCOSUTILSLIB = elvutils

OPENSSL=-L ../openssl
#OPENSSL=

# Inlcude dirs for this target (libLanDeviceUtils.so)
INCL = -Iinclude -Isrc


TARGETOBJS = \
	src/Constants.o \
	src/EQ3ConfigProtocol.o \
	src/IPConfiguration.o \
	src/LanDevice.o \
	src/LanDeviceUtils.o \
	src/LanifCfgProtocol.o \
	src/NetworkInterfaceHelper.o \
	src/Protocol.o \
	src/RuntimeIPConfiguration.o \
	src/TestStatusConfiguration.o \
	src/UDPDatagramSender.o \
	src/UtilityDeviceConfiguration.o \
	src/UtilityFirmwareUpdate.o \
	src/UtilityNetworkConfiguration.o \
	src/UtilitySearchDevices.o \
	src/UtilitySendCommand.o \
	src/TCPEncryption.o 

CC = $(CROSS)g++
CXX = $(CROSS)g++
STRIP = $(CROSS)strip

CPPFLAGS += -I./include -I./src -I$(BIDCOSUTILSPATH) -I../openssl/include -Wall $(MACHINE_OPTION) -fPIC

ifeq ($(CXXFLAGS),)
CXXFLAGS=$(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

LD_FLAGS += $(MACHINE_OPTION) -shared -fPIC -lpthread $(OPENSSL) -lcrypto -L$(BIDCOSUTILSPATH) -l$(BIDCOSUTILSLIB)

all: $(CONFIGURE) info bidcos-utils $(TARGET) 

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

bidcos-utils:
	make CROSS=$(CROSS) -C $(BIDCOSUTILSPATH)

bidcos-utils_clean:
	make CROSS=$(CROSS) -C $(BIDCOSUTILSPATH) clean


$(TARGET): $(TARGETOBJS) 
	$(CXX) -o $(TARGET) $(TARGETOBJS) $(LDFLAGS) $(LD_FLAGS)
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<


install: $(TARGET)
	$(ROMFSINST) /lib/$(TARGET)

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

clean:
	rm -vf src/*.o
	rm -vf config.mk
	rm -fv $(TARGET)
	rm -f $(TARGET).debug


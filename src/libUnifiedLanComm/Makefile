# Makefile for libUnifiedLanComm

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

TARGET = libUnifiedLanComm.so

LIBELVUTILSPATH = ../elvutils
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

#LIBHSSCOMMPATH = ../libs/libhsscomm
#LIBHSSCOMMPATH = $(LIBHSSCOMM_DIR)
#LIBHSSCOMM = hsscomm

#LIBXMLPARSERPATH = ../libs/xmlparser
#LIBXMLPARSERPATH = $(XMLPARSER_DIR)
#LIBXMLPARSER = xmlparser

#XMLRPCPATH = ../base/libs/xmlrpc
#XMLRPCPATH = $(XMLRPC_DIR)
#XMLRPCLIB = libXmlRpc.so
#XMLRPCINCLUDE = $(XMLRPCPATH)/src

LIBLANDEVICEUTILS_PATH = ../libLanDeviceUtils
#LIBLANDEVICEUTILS_PATH = $(LIBLANDEVICEUTILS_DIR)
LIBLANDEVICEUTILS = LanDeviceUtils
LIBLANDEVICEUTILS_INCLUDE = $(LIBLANDEVICEUTILS_PATH)/include

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0
else
OPTIMIZE = -O2 -g
endif

TARGETOBJS = \
	src/LanConnection.o \
	src/CommonConversion.o \
	src/UnifiedLanCommController.o \
	src/UnifiedLanProtocolMessage.o \
	src/UnifiedLanProtocolTypeConverter.o \
	src/LanConnectionListener.o

CC = $(CROSS)g++
CXX = $(CROSS)g++
STRIP = $(CROSS)strip

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CPPFLAGS += -I./src -I./include -I$(LIBELVUTILSPATH) -I$(LIBLANDEVICEUTILS_INCLUDE) -fPIC $(MACHINE_OPTION) -Wall

LD_FLAGS += $(MACHINE_OPTION) -shared -fPIC -lpthread -L$(LIBLANDEVICEUTILS_PATH) -l$(LIBLANDEVICEUTILS) -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) 

all: $(CONFIGURE) info liblandeviceutils $(TARGET) 

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

test: test_configure all 
	make -C ./test all

test_configure:
	make -C ./test configure

test_clean:
	make -C ./test clean

liblandeviceutils_clean:
	make CROSS=$(CROSS) -C $(LIBLANDEVICEUTILS_PATH) clean

liblandeviceutils:
	make CROSS=$(CROSS) -C $(LIBLANDEVICEUTILS_PATH) lib$(LIBLANDEVICEUTILS).so

liblandeviceutils_install:
	make CROSS=$(CROSS) -C $(LIBLANDEVICEUTILS_PATH) install

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

$(TARGET): $(TARGETOBJS)
	$(CXX) -o $(TARGET) $(TARGETOBJS) $(LDFLAGS) $(LD_FLAGS)
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

install: $(TARGET) 
	$(ROMFSINST) /lib/$(TARGET)


clean: liblandeviceutils_clean
	rm -rf src/*.o
	rm -f config.mk
	rm -rf $(TARGET)
	rm -f $(TARGET).debug	



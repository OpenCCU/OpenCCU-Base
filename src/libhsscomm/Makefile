# Makefile for libhsscomm

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

ROMFSINST = $(shell if [ -x ../../helper_scripts/romfs-inst.sh ]; then echo ../../helper_scripts/romfs-inst.sh; else echo /usr/bin/romfs-inst.sh; fi)

TARGET = libhsscomm.so

BASELIBDIR =  $(shell if [ -d ../../base/libs/elvutils ] ; then echo ../../base/libs; else echo ../../libs; fi)

ifdef ELVUTILS_DIR
LIBELVUTILSPATH = $(ELVUTILS_DIR)
else
LIBELVUTILSPATH = ../elvutils
endif
LIBELVUTILS = elvutils

ifdef XMLPARSER_DIR
LIBXMLPARSERPATH = $(XMLPARSER_DIR)
else
LIBXMLPARSERPATH = ../xmlparser
endif
LIBXMLPARSER = xmlparser

ifdef XMLRPC_DIR
XMLRPCPATH = $(XMLRPC_DIR)
else
XMLRPCPATH = ../xmlrpc
endif

XMLRPCLIB = libXmlRpc.so
XMLRPCINCLUDE = $(XMLRPCPATH)/src

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0 -pg
else
OPTIMIZE = -O2 -g
endif

TARGETOBJS = \
	CommController.o CommMessage.o CommMessageQueue.o PortWrapper.o \
	SocketPortWrapper.o UnixSerialPortWrapper.o AsyncXmlRpcSender.o ValueStore.o\
	FrameDescription.o LogicalInstance.o \
	type_registry.o \
	HSSDescription.o \
	HSSIntegerValueMap.o \
	HSSLogicalType.o \
	HSSLogicalTypeAction.o \
	HSSLogicalTypeBoolean.o \
	HSSLogicalTypeFloat.o \
	HSSLogicalTypeInteger.o \
	HSSLogicalTypeOption.o \
	HSSLogicalTypeString.o \
	HSSManager.o \
	HSSParameter.o \
	HSSParamset.o \
	HSSPhysicalDataInterface.o \
	HSSPhysicalDataInterfaceInternal.o \
	HSSPhysicalType.o \
	HSSPhysicalTypeArray.o \
	HSSPhysicalTypeBoolean.o \
	HSSPhysicalTypeInteger.o \
	HSSPhysicalTypeString.o \
	HSSTypeConversionActionInteger.o \
	HSSTypeConversionBooleanInteger.o \
	HSSTypeConversionFloatConfigtime.o \
	HSSTypeConversionFloatInteger.o \
	HSSTypeConversionStringUnsignedInteger.o \
	HSSTypeConversion.o \
	HSSTypeConversionAnyKeyCounter.o \
	HSSTypeConversionSameKeyCounter.o \
	HSSTypeConversionIdentity.o \
	HSSTypeConversionIntegerIntegerMap.o \
	HSSTypeConversionIntegerIntegerScale.o \
	HSSTypeConversionIntegerTinyfloat.o \
	HSSTypeConversionToggle.o \
	HSSTypeConversionRc19Display.o \
	HSSTypeConversionCfm.o \
	HSSTypeConversionCCRTDNParty.o \
	HSSTypeConversionBlindTest.o \
	HSSTypeConversionHexStringToByteArray.o \
	HSSTypeConversionInt4ToSignedIntX.o \
	HSSTypeConversionFloatUInt8String.o \
	MetadataStore.o \
	StructuredFrame.o \
	HSSXmlRpcEventDispatcher.o \
	HSSXmlRpcEventMessage.o \
	
	

CC = $(CROSS)g++
CXX = $(CROSS)g++
LD = $(CROSS)g++

STRIP = $(CROSS)strip

CPPFLAGS += -I$(LIBELVUTILSPATH) -I$(LIBXMLPARSERPATH) -I$(XMLRPCINCLUDE) -I$(TCLINCLUDEPATH) -Wall -fPIC

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(MACHINE_OPTION) $(OPTIMIZE) 
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

LD_FLAGS += $(MACHINE_OPTION) -shared -fPIC -lpthread -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) -L$(LIBXMLPARSERPATH) -l$(LIBXMLPARSER) -L$(XMLRPCPATH) -lXmlRpc 

all: $(CONFIGURE) info $(TARGET) 

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

$(TARGET): $(TARGETOBJS)
	$(CXX) $(LDFLAGS) $(LD_FLAGS) -o $(TARGET) $(TARGETOBJS)
	cp -f $(TARGET) $(TARGET).debug
	$(STRIP) $(TARGET)

$(TARGETOBJS):
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $(patsubst %.o, %.cpp, $@)
	
install: $(TARGET) 
	$(ROMFSINST) /lib/$(TARGET)

package-ccu2:
	install -m 0755 -d $(CCU2_LIB_DIR)
	install -m 0755 $(TARGET) $(CCU2_LIB_DIR)
	
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
	rm -rf *.o
	rm -f config.mk 
	rm -rf $(TARGET)
	rm -f $(TARGET).debug

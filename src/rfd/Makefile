# Makefile for rfd

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

ROMFSINST = /usr/bin/romfs-inst.sh

TARGET = rfd

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


LIBLANDEVICEUTILSPATH = ../libLanDeviceUtils
#LIBLANDEVICEUTILSPATH = $(LIBLANDEVICEUTILS_DIR)
LIBLANDEVICEUTILS = LanDeviceUtils
LIBLANDEVICEUTILSINCLUDE = $(LIBLANDEVICEUTILSPATH)/include


LIBUNIFIEDLANCOMMPATH = ../libUnifiedLanComm
#LIBUNIFIEDLANCOMMPATH = $(LIBUNIFIEDLANCOMM_DIR)
LIBUNIFIEDLANCOMM = UnifiedLanComm
LIBUNIFIEDLANCOMMINCLUDE = $(LIBUNIFIEDLANCOMMPATH)/include

OPENSSLDIR = ../openssl
#OPENSSL=-L $(OPENSSLDIR)
OPENSSL=
#OPENSSLINCLUDE=-I$(OPENSSLDIR)/include
OPENSSLINCLUDE=

CCU2INCLUDE = ./hm2

ifneq ($(HAS_USB_SUPPORT),0)
#LIBUSBPATH = /usr/include/libusb-1.0
LIBUSBPATH = ../libusb/libusb
#LIBUSBPATH = $(LIBUSB_DIR)/libusb
LIBUSBLIBPATH = $(LIBUSBPATH)/.libs
LIBUSB = usb-1.0
TARGETS_USB = hid_linux/HIDDevice.o hid_linux/HIDReportDescriptor.o BidcosUsbInterface.o BidcosUsbConnection.o
CXXFLAGS_USB += -Ihid_linux -I$(LIBUSBPATH) -DHAS_USB_SUPPORT
LDFLAGS_USB = -L$(LIBUSBLIBPATH) -l$(LIBUSB)
endif

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0 -pg
else
OPTIMIZE = -O2 -g
endif

ifeq ($(PROJECT_VERSION),)
PROJECT_VERSION = $(shell ./version.sh)
endif

ifeq "$(PRODUCTION)" "1"
RFD_DEFINES="\#define PRODUCTION 1"
else
RFD_DEFINES=""
endif

TARGETOBJS = $(TARGET).o \
	BidcosFrame.o \
	BidcosFrameDecoder.o \
	BidcosFrameDetermineValue.o \
	BidcosFrameEnterConfig.o \
	BidcosFrameLinkPeerReq.o \
	BidcosFrameParamReq.o \
	BidcosInterface.o \
	BidcosInterfaceConcentrator.o \
	BidcosInterfaceConnection.o \
	BidcosInterfaceMessage.o \
	BidcosLanConnection.o \
	BidcosLanInterface.o \
	BidcosRemoteInterface.o \
	RFCentral.o \
	RFChannel.o \
	RFChannelDescription.o \
	RFCommMessage.o \
	RFCommMessageAESKey.o \
	RFCommMessageDecoder.o \
	RFConfigData.o \
	RFController.o \
	RFDevice.o \
	RFDeviceDescription.o \
	RFLoggingManager.o \
	RFLogicalInstance.o \
	RFMaintenanceChannel.o \
	RFManager.o \
	RFParamset.o \
	RFPhysicalDataInterfaceCommand.o \
	RFPhysicalDataInterfaceCentralCommand.o \
	RFPhysicalDataInterfaceConfig.o \
	RFPhysicalDataInterfaceConfigString.o \
	RFPhysicalDataInterfaceStore.o \
	RFPhysicalDataInterfaceMultiframeCommand.o \
	RFSystemDescription.o \
	RFTeam.o \
	XmlRpcMethods.o \
	hm2/CCU2PortWrapper.o \
	hm2/LGWPortWrapper.o \
	hm2/FileIO.o \
	hm2/HM2Utils.o \
	hm2/CCU2SerialFrame.o \
	hm2/CCU2SerialPortWrapper.o \
	hm2/CCU2SerialPortWrapperLinux.o \
	hm2/CCU2BidcosRemoteInterface.o \
	hm2/CCU2CoprocessorCommand.o \
	hm2/CCU2CommController.o \
	hm2/CCU2SerialPortCommController.o \
	hm2/CCU2LGWCommController.o \
	RFFirmwareManager.o\
	RFReplaceMap.o\
	BidcosFrameStartBootloader.o\
	JSONObject.o\
	$(TARGETS_USB)
	

CC = $(CROSS)g++
CXX = $(CROSS)g++
STRIP = $(CROSS)strip

LD_FLAGS += --verbose -lpthread -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) -L$(LIBHSSCOMMPATH) -l$(LIBHSSCOMM) \
			-L$(LIBXMLPARSERPATH) -l$(LIBXMLPARSER) -L$(XMLRPCPATH) -lXmlRpc $(LDFLAGS_USB) \
			-L$(LIBLANDEVICEUTILSPATH) -l$(LIBLANDEVICEUTILS) -L$(LIBUNIFIEDLANCOMMPATH) -l$(LIBUNIFIEDLANCOMM) $(OPENSSL) -lcrypto -lc -lm -std=c++11

CPPFLAGS = -I$(LIBELVUTILSPATH) -I$(LIBHSSCOMMPATH) -I$(LIBXMLPARSERPATH) -I$(XMLRPCINCLUDE) \
			-I$(LIBLANDEVICEUTILSINCLUDE) -I$(LIBUNIFIEDLANCOMMINCLUDE) -I$(CCU2INCLUDE) -I./ $(CXXFLAGS_USB) $(OPENSSLINCLUDE) -Wall 

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

ifneq ($(XMLRPC_DEBUG),)
CXXFLAGS += -DXMLRPC_DEBUG
endif


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
	echo "#define RFD_VERSION \"$(PROJECT_VERSION)\"" > generated-rfd-version.h
	echo $(RFD_DEFINES) > hm2/RFDDefines.h
	
$(TARGET): $(TARGETOBJS)
	$(CXX) -o $(TARGET) $(TARGETOBJS) $(LD_FLAGS) $(LDFLAGS) 
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET) 


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

install: $(TARGET)
	$(ROMFSINST) /bin/$(TARGET)

clean:
	rm -rf *.o
	rm -f config.mk 
	rm -rf $(TARGET)
	rm -f hm2/*.o
	rm -f hid_linux/*.o
	rm -f $(TARGETS_USB)
	rm -f $(TARGET).debug

test: test_configure all
	make -C ./test
	
test_configure:
	make -C ./test configure

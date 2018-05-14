#*******************************************************************************
#* Makefile for HSS LED Firmware 
#*******************************************************************************

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

TARGET = hss_led

LIBELVUTILSPATH = ../elvutils
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

LIBXMLPARSERPATH = ../xmlparser
#LIBXMLPARSERPATH = $(XMLPARSER_DIR)
LIBXMLPARSER = xmlparser

XMLRPCPATH = ../xmlrpc
#XMLRPCPATH = $(XMLRPC_DIR)
XMLRPCLIB = XmlRpc
XMLRPCINCLUDE = $(XMLRPCPATH)/src

OBJS = $(TARGET).o\
	led.o \
	UdpCannel.o \
	MessageParser.o \
	AlarmMessage.o \
	Info.o \
	InfoLed.o \
	InternetLed.o \
	Network.o \
	ServiceMessage.o \
	UpdateAvailable.o \
	RFLGWInfoLED.o
	
				
#all: $(TARGET)
all: $(CONFIGURE) info $(TARGET)

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)
	
CC = $(CROSS)g++
CXX = $(CROSS)g++
STRIP = $(CROSS)strip

LD_FLAGS += -L$(XMLRPCPATH) -l$(XMLRPCLIB) -L$(LIBELVUTILSPATH) -L$(LIBXMLPARSERPATH) -l$(LIBELVUTILS) -l$(LIBXMLPARSER) -lpthread
CPPFLAGS += -I$(LIBELVUTILSPATH) -I$(LIBXMLPARSERPATH) -I$(XMLRPCINCLUDE) -Wall
	
install: $(TARGET)
	$(ROMFSINST) -p 755 $(TARGET) /bin/$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LD_FLAGS) -o $(TARGET) $(OBJS)
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
	
info:
	@echo ""
	@echo "**************************"
	@echo "* MODULE $(TARGET)"
	@echo "**************************"
	@echo "* PROJECT_DIR 		= $(PROJECT_DIR)"
	@echo "* TARGET_DIR  		= $(TARGET_DIR)"
	@echo "* BIN_DIR     		= $(BIN_DIR)"
	@echo "**************************"
	@echo ""
	
clean:
	rm -rf *.o
	rm -rf config.mk
	rm -rf target
	rm -rf $(TARGET)




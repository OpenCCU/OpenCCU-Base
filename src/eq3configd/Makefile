# Makefile for eQ-3ConfigD

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

TARGET = eq3configd

LIBELVUTILSPATH = ../elvutils
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

OPENSSL=-L ../openssl
#OPENSSL=
OPENSSLINCLUDE=-I../openssl/include
#OPENSSLINCLUDE=

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0 -pg
else
OPTIMIZE = -O2
endif

TARGETOBJS = main.o \
	src/udpframe.o \
	src/networkconfig.o 

CC = $(CROSS)g++
CXX = $(CROSS)g++
ifeq "$(DEBUG)" "1"
STRIP = true
else
STRIP = $(CROSS)strip
endif

ifneq ($(XMLRPC_DEBUG),)
CXX_FLAGS += -DXMLRPC_DEBUG
endif

CXX_FLAGS +=  $(OPTIMIZE) -Wall -I$(LIBELVUTILSPATH) $(OPENSSLINCLUDE) -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64
LD_FLAGS += -lpthread -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) $(OPENSSL) -l:libcrypto.so.4

all: $(CONFIGURE) info $(TARGET)

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

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
	$(CXX) -o $(TARGET) $(TARGETOBJS) $(LD_FLAGS)
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

install: $(TARGET) 
	$(STRIP) $(TARGET)
	$(ROMFSINST) /bin/$(TARGET)

clean:
	rm -rf *.o
	rm -rf config.mk
	rm -rf $(TARGET)
	rm -rf src/*.o

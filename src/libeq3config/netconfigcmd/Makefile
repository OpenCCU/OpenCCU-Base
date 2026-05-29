# Makefile for netconfigcmd

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

TARGET = libnetconfigcmd.a 

LIBELVUTILSPATH = $(realpath ../../../elvutils)
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0 -pg
else
OPTIMIZE = -O2 -g
endif

TARGETOBJS = netconfigcmd.o \
	src/udpframe.o \
	src/networkconfig.o 

CXX = $(CROSS)g++
AR=$(CROSS)ar

ifeq "$(DEBUG)" "1"
STRIP = true
else
STRIP = $(CROSS)strip
endif

INCLUDEDIRS= -I./ -I../ -I$(LIBELVUTILSPATH)

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CPPFLAGS += $(INCLUDEDIRS) -fPIC  -Wall 


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
	$(AR) rcsv $(TARGET) $(TARGETOBJS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

#install: $(TARGET) $(TARGETCMD)
#	$(STRIP) $(TARGET)
#	$(ROMFSINST) /bin/$(TARGET)
#	$(STRIP) $(TARGETCMD)
#	$(ROMFSINST) /bin/$(TARGETCMD)

clean:
	rm -rf *.o
	rm -rf $(TARGET)
	rm -rf src/*.o

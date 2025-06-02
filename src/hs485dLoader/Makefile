#*******************************************************************************
#* Makefile for hs485dLoader
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

LIBELVUTILSPATH = ../elvutils
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

LIBXMLPARSERPATH = ../xmlparser
#LIBXMLPARSERPATH = $(XMLPARSER_DIR)
LIBXMLPARSER = xmlparser

TARGET = hs485dLoader

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0
else
OPTIMIZE = -O2
endif

ifeq "$(DEBUG)" "1"
STRIP = true
else
STRIP = $(CROSS)strip
endif

CC = $(CROSS)g++
CXX = $(CROSS)g++

TARGETOBJS = \
	$(TARGET).o 

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CPPFLAGS += -I$(LIBELVUTILSPATH) -I$(LIBXMLPARSERPATH) -Wall
LD_FLAGS += -lpthread -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) -L$(LIBXMLPARSERPATH) -l$(LIBXMLPARSER) -lc -lm
				
all: $(CONFIGURE) info $(TARGET)

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

$(TARGET): $(TARGETOBJS)
	$(CXX) -o $(TARGET) $(TARGETOBJS) $(LD_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
	
install: $(TARGET)
	$(STRIP) $(TARGET)
	$(ROMFSINST) -p 755 $(TARGET) /bin/$(TARGET)

	
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
	rm -rf $(TARGET)


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

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0 -pg
else
OPTIMIZE = -O2 -g
endif

#---------------
# TARGET/-FILES
#---------------

TARGET=CCU2CommControllerMod

TARGETOBJS= \
	CCU2CommControllerMod.o \
	CCU2CoprocessorCommandMod.o \
	CCU2LGWCommControllerMod.o \
	CCU2PortWrapperMod.o \
	CCU2SerialFrameMod.o \
	CCU2SerialPortCommControllerMod.o \
	CCU2SerialPortWrapperLinuxMod.o \
	CCU2SerialPortWrapperMod.o \
	FileIOMod.o \
	HM2UtilsMod.o \
	LGWPortWrapperMod.o

#--------
# FLAGS
#--------

LIBELVUTILSPATH = $(realpath ../../../elvutils)
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

LIBLANDEVICEUTILSPATH = $(realpath ../../../libLanDeviceUtils)
#LIBLANDEVICEUTILSPATH = $(LIBLANDEVICEUTILS_DIR)
LIBLANDEVICEUTILS = LanDeviceUtils
LIBLANDEVICEUTILSINCLUDE = $(LIBLANDEVICEUTILSPATH)/include


LIBUNIFIEDLANCOMMPATH = $(realpath ../../../libUnifiedLanComm)
#LIBUNIFIEDLANCOMMPATH = $(LIBUNIFIEDLANCOMM_DIR)
LIBUNIFIEDLANCOMM = UnifiedLanComm
LIBUNIFIEDLANCOMMINCLUDE = $(LIBUNIFIEDLANCOMMPATH)/include
	
INCLUDE = -I ./ -I$(LIBELVUTILSPATH) -I$(LIBLANDEVICEUTILSINCLUDE) -I$(LIBUNIFIEDLANCOMMINCLUDE)

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CPPFLAGS += -fPIC $(INCLUDE) 

CXX=$(CROSS)g++
AR=$(CROSS)ar

#---------
# TARGETS
#---------

all: configure info $(TARGET)
	echo "-L $(LIBELVUTILSPATH) -l $(LIBELVUTILS) -L$(LIBLANDEVICEUTILSPATH) -l$(LIBLANDEVICEUTILS) -L$(LIBUNIFIEDLANCOMMPATH) -l$(LIBUNIFIEDLANCOMM)" >> ../modulesDependantLibs.dep

$(TARGET): $(TARGETOBJS)
	$(AR) rcsv lib$(TARGET).a $(TARGETOBJS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

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
	@echo "* PROJECT_DIR 		= $(PROJECT_DIR)"
	@echo "* TARGET_DIR  		= $(TARGET_DIR)"
	@echo "* BIN_DIR     		= $(BIN_DIR)"
	@echo "**************************"
	@echo ""
	
clean:
	rm -f *.o
	rm -f *.a
	rm -f $(TARGET)

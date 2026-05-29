

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

TARGET=test-coprocessor

TARGETOBJS = test-coprocessor.o 

LIBLANDEVICEUTILSPATH = $(realpath ../../../libLanDeviceUtils)
#LIBLANDEVICEUTILSPATH = $(LIBLANDEVICEUTILS_DIR)
LIBLANDEVICEUTILS = LanDeviceUtils
LIBLANDEVICEUTILSINCLUDE = $(LIBLANDEVICEUTILSPATH)/include

LIBUNIFIEDLANCOMMPATH = $(realpath ../../../libUnifiedLanComm)
#LIBUNIFIEDLANCOMMPATH = $(LIBUNIFIEDLANCOMM_DIR)
LIBUNIFIEDLANCOMM = UnifiedLanComm
LIBUNIFIEDLANCOMMINCLUDE = $(LIBUNIFIEDLANCOMMPATH)/include

LIBELVUTILSPATH = $(realpath ../../../elvutils)
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

CCU2COMMCONTROLLERMODPATH = $(shell pwd)/../CCU2CommControllerMod
CCU2COMMCONTROLLERMODINCLUDE = $(CCU2COMMCONTROLLERMODPATH)
CCU2COMMCONTROLLERMODLIB = CCU2CommControllerMod
	
CPPFLAGS= -fPIC -I./ -I../ -I$(LIBLANDEVICEUTILSINCLUDE) -I$(LIBUNIFIEDLANCOMMINCLUDE) -I$(LIBELVUTILSPATH) -I$(CCU2COMMCONTROLLERMODINCLUDE) -Wall

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CXX=$(CROSS)g++
AR=$(CROSS)ar


all: $(CONFIGURE) info depends $(TARGET)
	echo "-L $(LIBLANDEVICEUTILSPATH) -l $(LIBLANDEVICEUTILS) -L $(LIBUNIFIEDLANCOMMPATH) -l $(LIBUNIFIEDLANCOMM) -L $(LIBELVUTILSPATH) -l $(LIBELVUTILS) -L$(CCU2COMMCONTROLLERMODPATH) -l$(CCU2COMMCONTROLLERMODLIB)" >> ../modulesDependantLibs.dep

		
clean: clean_depends
	rm -f *.o
	rm -f *.a
	rm -f *.out
	
clean_depends:
	make -C ../CCU2CommControllerMod clean	
	make -C $(LIBLANDEVICEUTILSPATH) clean
	make -C $(LIBUNIFIEDLANCOMMPATH) clean
	make -C $(LIBELVUTILSPATH) clean

	
configure: configure_depends
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

configure_depends:
	make -C ../CCU2CommControllerMod configure

depends:
	make -C ../CCU2CommControllerMod
	make -C $(LIBLANDEVICEUTILSPATH) 
	make -C $(LIBUNIFIEDLANCOMMPATH)
	make -C $(LIBELVUTILSPATH) 

$(TARGET): $(TARGETOBJS)
	$(AR) rcsv lib$(TARGET).a $(TARGETOBJS) $(CCU2COMMCONTROLLERMODPATH)/lib$(CCU2COMMCONTROLLERMODLIB).a

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


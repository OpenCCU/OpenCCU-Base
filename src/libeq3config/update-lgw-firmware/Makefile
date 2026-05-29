

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

TARGET=update-lgw-firmware

TARGETOBJS= \
	$(TARGET).o \
	../lgwallocation/lgwallocation.o

LIBLANDEVICEUTILSPATH = $(realpath ../../../libLanDeviceUtils)
#LIBLANDEVICEUTILSPATH = $(LIBLANDEVICEUTILS_DIR)
LIBLANDEVICEUTILS = LanDeviceUtils
LIBLANDEVICEUTILSINCLUDE = $(LIBLANDEVICEUTILSPATH)/include

LIBELVUTILSPATH = $(realpath ../../../elvutils)
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

CPPFLAGS= -fPIC -I../ -I$(LIBLANDEVICEUTILSINCLUDE) -I$(LIBELVUTILSPATH) -I../lgwallocation

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CXX=$(CROSS)g++
AR=$(CROSS)ar


all: $(CONFIGURE) info $(TARGETOBJS)
	$(AR) rcsv lib$(TARGET).a $(TARGETOBJS) 
	echo "-L $(LIBLANDEVICEUTILSPATH) -l $(LIBLANDEVICEUTILS) -L $(LIBELVUTILSPATH) -l $(LIBELVUTILS)" >> ../modulesDependantLibs.dep

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
		
clean: clean_depends
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f ../lgwallocation/*.o
  
clean_depends:
	$(MAKE) -C $(LIBLANDEVICEUTILSPATH) clean


	
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


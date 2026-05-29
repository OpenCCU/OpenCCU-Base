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

TARGET=read-default-rf-address

TARGETOBJS= ReadDefaultRFAddress.o 

#--------
# FLAGS
#--------

LIBELVUTILSPATH = $(realpath ../../../elvutils)
#LIBELVUTILSPATH = $(ELVUTILS_DIR)
LIBELVUTILS = elvutils

CCU2COMMCONTROLLERMODPATH = $(shell pwd)/../CCU2CommControllerMod
CCU2COMMCONTROLLERMODINCLUDE = $(CCU2COMMCONTROLLERMODPATH)
CCU2COMMCONTROLLERMODLIB = CCU2CommControllerMod
	
INCLUDE = -I ./ -I ../ -I$(LIBELVUTILSPATH) -I$(CCU2COMMCONTROLLERMODINCLUDE)

CPPFLAGS = -fPIC $(INCLUDE)

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

LD_FLAGS = -lpthread

CXX=$(CROSS)g++
AR=$(CROSS)ar

#---------
# TARGETS
#---------

all: configure info depends $(TARGET)
	echo "-L $(LIBELVUTILSPATH) -l $(LIBELVUTILS) -L$(CCU2COMMCONTROLLERMODPATH) -l$(CCU2COMMCONTROLLERMODLIB)" >> ../modulesDependantLibs.dep

$(TARGET): $(TARGETOBJS)
	$(AR) rcsv lib$(TARGET).a $(TARGETOBJS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

configure: depends_configure
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

depends_configure:
	make -C $(CCU2COMMCONTROLLERMODPATH) configure

depends: 
	make -C $(CCU2COMMCONTROLLERMODPATH)
	
clean_depends:
	make -C $(CCU2COMMCONTROLLERMODPATH) clean	
	
clean: clean_depends
	rm -f *.o
	rm -f *.a
	rm -f $(TARGET)

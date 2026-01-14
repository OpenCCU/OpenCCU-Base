#Makefile for eq3configcmd

#List of modules for eqconfigcmd
#Set default modules if not already defined
ifeq ($(EQ3CONFIG_MODULES),)
EQ3CONFIG_MODULES=netconfigcmd update-lgw-firmware update-coprocessor setlgwkey rfd-interface-copy read-default-rf-address wait-for-file test-coprocessor
export EQ3CONFIG_MODULES
endif

EQ3CONFIGCMDMODULES_PATH=../eq3config-modules
EQ3CONFIGCMDMODULES_LIB=eq3config

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

STRIP = $(CROSS)strip

ROMFSINST = /usr/bin/romfs-inst.sh

LD_FLAGS= -L $(EQ3CONFIGCMDMODULES_PATH) -l $(EQ3CONFIGCMDMODULES_LIB) -l pthread -L ../../openssl -lcrypto

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CPPFLAGS += -I $(EQ3CONFIGCMDMODULES_PATH)

TARGET=eq3configcmd

TARGETOBJS= \
	eq3configcmd.o 


CXX = $(CROSS)g++
LD = $(CROSS)g++


all: $(CONFIGURE) info eq3configcmdlib $(TARGETOBJS) 
	#echo "EQ3CONFIG_MODULE_DEPENDANT_LIBS=$(EQ3CONFIG_MODULE_DEPENDANT_LIBS)"
	$(LD) -o $(TARGET) $(TARGETOBJS) $(LD_FLAGS) $(LDFLAGS) $(shell cat $(EQ3CONFIGCMDMODULES_PATH)/modulesDependantLibs.dep) -lrt 
	cp -f $(TARGET) $(TARGET).debug || true
	$(STRIP) $(TARGET)
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

eq3configcmdlib:
	$(MAKE) -C $(EQ3CONFIGCMDMODULES_PATH) 	
	
	
configure: configure_eq3configcmdlib
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

configure_eq3configcmdlib:
	$(MAKE) PROJECT_DIR=$(PROJECT_DIR) PLATFORM=$(PLATFORM) configure -C $(EQ3CONFIGCMDMODULES_PATH)

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
	
install: $(TARGET) eq3configcmdlib_install 
	$(ROMFSINST) /bin/$(TARGET)	

clean: eq3configcmdlib_clean
	rm -f config.mk
	rm -f $(TARGET)
	rm -f $(TARGETOBJS)
	rm -f $(TARGET).debug

eq3configcmdlib_install: eq3configcmdlib
	$(MAKE) -C $(EQ3CONFIGCMDMODULES_PATH) install
eq3configcmdlib_clean :
	$(MAKE) -C $(EQ3CONFIGCMDMODULES_PATH) clean


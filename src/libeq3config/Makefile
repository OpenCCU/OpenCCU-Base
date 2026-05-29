# Makefile for eq3configcmd-modules

# Is EQ3CONFIG_MODULES specified?
ifeq ($(EQ3CONFIG_MODULES),)
$(error "EQ3CONFIG_MODULES not set.")
endif

#$(patsubst module_%, %, $@)
MODULEINCLUDES=$(patsubst %, -I%, $(EQ3CONFIG_MODULES))

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

CPPFLAGS= -I./ -fPIC $(MODULEINCLUDES) -ICCU2CommControllerMod 

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64

CXX=$(CROSS)g++
LD=$(CROSS)g++

CC_HOST=g++
LD_HOST=g++

TARGETOBJS= \
	CommandManager.o \
	Command.o \
	Commands.o \
	HelpSystem.o

TARGET=libeq3config.so


all: $(CONFIGURE) info module_depends commands modules $(TARGET) 

clean: modules_clean commandsgenerator_clean
	rm -f $(TARGET)
	rm -f $(TARGETOBJS)
	rm -f $(TARGET).debug

commands: commandsgenerator
	echo "#include <HelpSystem.h>\nstatic HelpSystem helpSystemInstance;\n" > Commands.cpp;
	./commandsgenerator $(patsubst %, %/module.def,$(EQ3CONFIG_MODULES));
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -oCommands.o Commands.cpp 

commandsgenerator: 
	$(CC_HOST) -o commandsgenerator commandsgenerator.cpp
	
commandsgenerator_clean:
	rm -f commandsgenerator
	rm -f commandsgenerator.o
	rm -f Commands.cpp

module_depends: 
	echo "" > modulesDependantLibs.dep
	
modules: $(patsubst %, module_%, $(EQ3CONFIG_MODULES))

modules_clean: $(patsubst %, module_%_clean, $(EQ3CONFIG_MODULES))

$(patsubst %, module_%, $(EQ3CONFIG_MODULES)):
	make -C $(patsubst module_%, %, $@)

$(patsubst %, module_%_clean, $(EQ3CONFIG_MODULES)):
	make -C $(patsubst module_%_clean, %, $@) clean
	
$(TARGET): $(TARGETOBJS)
	$(LD) -shared $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(TARGETOBJS) $(patsubst %, -L %, $(EQ3CONFIG_MODULES)) $(patsubst %, -l %, $(EQ3CONFIG_MODULES))	
	cp -f $(TARGET) $(TARGET).debug 
	$(CROSS)strip $(TARGET)
	
%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<	

install: $(TARGET) 
	$(ROMFSINST) /lib/$(TARGET)	


configure: configure_modules
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)


configure_modules: $(patsubst %, module_%_configure, $(EQ3CONFIG_MODULES))

$(patsubst %, module_%_configure, $(EQ3CONFIG_MODULES)):
	$(MAKE) PROJECT_DIR=$(PROJECT_DIR) PLATFORM=$(PLATFORM) configure -C $(patsubst module_%_configure, %, $@)
	
	
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

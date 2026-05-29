# Makefile for libelvutils

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

ifeq ($(PLATFORM),dtag)
TIMESTAMP_O = TimestampLinuxDtag.o
else
TIMESTAMP_O = TimestampLinux.o
endif

ROMFSINST = $(shell if [ -x ../../helper_scripts/romfs-inst.sh ]; then echo ../../helper_scripts/romfs-inst.sh; else echo /usr/bin/romfs-inst.sh; fi)

ELVUTILS_SO = libelvutils.so
ELVUTILS_A = libelvutils.a

ELVUTILSOBJS = \
	ConsoleLogger.o \
	FileLogger.o \
	Hexfile.o \
	Logger.o \
	OSCompatLinux.o \
	md5.o \
	MulticastSender.o \
	PropertyMap.o \
	SyslogLogger.o \
	TimerQueue.o \
	TimerTarget.o \
	TimeZoneInfo.o \
	unsigned_char_traits.o \
	utils.o \
	UpdateFile.o\
	Crc16.o\
	Actor.o \
	ActorMessage.o \
	PThreadCondition.o \
	$(TIMESTAMP_O) \
	
ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0 -pg
else
OPTIMIZE = -O2 -g
endif

STRIP = $(CROSS)strip

CC = $(CROSS)g++
CXX = $(CROSS)g++
AR = $(CROSS)ar
LD = $(CROSS)g++
ARFLAGS = rcs

ifeq ($(CXXFLAGS),)
CXXFLAGS = $(OPTIMIZE)
endif
CXXFLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64 -Wall -fPIC $(MACHINE_OPTION)

all: $(CONFIGURE) info $(ELVUTILS_SO) $(ELVUTILS_A) 

configure:
	rm -f $(LOCAL_CONFIG_FILE)
	touch $(LOCAL_CONFIG_FILE)
	echo "PROJECT_DIR=$(PROJECT_DIR)" >> $(LOCAL_CONFIG_FILE)
	echo "PLATFORM=$(PLATFORM)" >> $(LOCAL_CONFIG_FILE)

$(ELVUTILS_SO): $(ELVUTILSOBJS)
	$(CXX) $(LDFLAGS) -shared -fPIC $(MACHINE_OPTION) -o $(ELVUTILS_SO) $(ELVUTILSOBJS) $(OPENSSL) -lcrypto
	cp -f $(ELVUTILS_SO) $(ELVUTILS_SO).debug || true
	$(STRIP) $(ELVUTILS_SO)


$(ELVUTILS_A): $(ELVUTILSOBJS)
	$(AR) $(ARFLAGS) $(ELVUTILS_A) $(ELVUTILSOBJS)


install: $(ELVUTILS_SO)
	$(ROMFSINST) /lib/$(ELVUTILS_SO)

package-ccu2:
	install -m 0755 -d $(CCU2_LIB_DIR)
	install -m 0755 $(ELVUTILS_SO) $(CCU2_LIB_DIR)
	
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

clean:
	rm -rf *.o
	rm -rf config.mk
	rm -rf $(ELVUTILS_SO)
	rm -rf $(ELVUTILS_A)
	rm -f $(ELVUTILS_SO).debug

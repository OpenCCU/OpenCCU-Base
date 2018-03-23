# Makefile for TARGET extension

ROMFSINST = /usr/bin/romfs-inst.sh
STRIP = $(CROSS)strip

TARGET = ssdpd

TARGETOBJS = $(TARGET.o)

LIBELVUTILSPATH = ../base/libs/elvutils
LIBELVUTILS = elvutils

CC = arm-linux-g++
CXX = arm-linux-g++
CXXFLAGS +=  -O2 -I$(LIBELVUTILSPATH) -Wall -Wstrict-prototypes
LDFLAGS += -L$(LIBELVUTILSPATH) -l$(LIBELVUTILS) -lpthread

all: $(TARGET)

$(TARGET): $(TARGETOBJS)

install: $(TARGET)
	$(STRIP) $(TARGET)
	$(ROMFSINST) /bin/$(TARGET)
	
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
	rm -rf $(TARGET)

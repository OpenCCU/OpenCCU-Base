# Makefile for TARGET extension

ROMFSINST = /usr/bin/romfs-inst.sh
STRIP = $(CROSS)strip

TARGET = tclticks.so

TCLPATH = ../base/tcl
TCLVERSION = $(shell awk -F "[ \t]*=[ \t]*" 'NF==2 && $$1 == "VERSION" {print $$2}' $(TCLPATH)/unix/Makefile)
TCLINCLUDEPATH = $(TCLPATH)/generic
TCLLIBPATH = $(TCLPATH)/unix
TCLLIB = -L$(TCLLIBPATH) -ltcl$(TCLVERSION)

TARGETOBJS = tclticks.o

CC = $(CROSS)g++
CXX = $(CROSS)g++
LD = $(CROSS)g++
CXXFLAGS +=  -fPIC -O2 -I$(TCLINCLUDEPATH) -Wall -Wstrict-prototypes
LDFLAGS += -shared -fPIC $(TCLLIB) -lstdc++

all: $(TARGET)

$(TARGET): $(TARGETOBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(TARGETOBJS)

install: $(TARGET)
	$(STRIP) $(TARGET)
	$(ROMFSINST) /lib/$(TARGET)


clean:
	rm -rf *.o
	rm -rf $(TARGET)

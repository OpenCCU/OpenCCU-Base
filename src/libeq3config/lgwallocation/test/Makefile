#Makefile for TestLGWAllocation

ifeq ($(GMOCK_DIR),)
$(error "Please provide GMOCK_DIR environment variable.")
endif

#ROMFSINST = $(shell if [ -x ../helper_scripts/romfs-inst.sh ]; then echo ../helper_scripts/romfs-inst.sh; else echo /usr/bin/romfs-inst.sh; fi)

TARGET = TestLGWAllocation

ifeq "$(DEBUG)" "1"
OPTIMIZE = -g -O0
else
OPTIMIZE = -O2
endif

TARGETOBJS = \
	src/TestSuiteLGWAllocation.o


CC = g++
CXX = g++
ifeq "$(DEBUG)" "1"
STRIP = true
else
STRIP = strip
endif

CXX_FLAGS += -std=c++11 -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64 $(OPTIMIZE) -I./src -I../ -I$(GMOCK_DIR)/include -I$(GMOCK_DIR)/gtest/include -fPIC $(MACHINE_OPTION) -Wall
LD_FLAGS += $(MACHINE_OPTION) -fPIC -L$(GMOCK_DIR) -lgmock -lpthread 

all: lgwallocation.o $(TARGET) 

lgwallocation_clean:
	rm -f lgwallocation.o
	
lgwallocation.o:
		$(CXX) $(CXX_FLAGS) -c ../lgwallocation.cpp

$(TARGET): $(TARGETOBJS) 
	$(CXX) -o $(TARGET) $(TARGETOBJS) lgwallocation.o  $(LD_FLAGS)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean: lgwallocation_clean
	rm -rf src/*.o
	rm -rf $(TARGET)


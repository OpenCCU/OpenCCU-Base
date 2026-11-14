# Binary distribution Makefile for libhsscomm

ROMFSINST = $(shell if [ -x ../../helper_scripts/romfs-inst.sh ]; then echo ../../helper_scripts/romfs-inst.sh; else echo /usr/bin/romfs-inst.sh; fi)

TARGET = libhsscomm.so

CROSS = arm-linux-

STRIP = $(CROSS)strip

all:
	#do nothing


install: $(TARGET)
	$(STRIP) $(TARGET)
	$(ROMFSINST) /lib/$(TARGET)


clean:
	#do nothing

#
#	Makefile -- Build instructions for ATMEL/AT91
#

ROMFS_DIRS = lib bin

#export ROMFSDIR

all:

install:
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROMFSINST) -p 755 /lib/libfirewall.tcl
	$(ROMFSINST) -p 755 /bin/setfirewall.tcl

clean:


TCL_LIB_DIR=$(ROMFSDIR)/lib/tcl8.2
LIB_DIR=$(TCL_LIB_DIR)/homematic

all:

install: homematic_tcl pkgIndex.tcl
	mkdir -p $(LIB_DIR)
	cp homematic_nocomments.tcl $(LIB_DIR)/homematic.tcl
	cp pkgIndex.tcl $(LIB_DIR)/pkgIndex.tcl
	chmod -R 755 $(LIB_DIR)
	
homematic_tcl: src/*
	./create_homematic_tcl.tcl
	./stripcomments.tcl homematic.tcl > homematic_nocomments.tcl
	
docu: homematic_tcl
	tcldoc docu homematic.tcl
	
clean:
	rm -f homematic.tcl
	rm -f homematic_nocomments.tcl
	rm -rf docu

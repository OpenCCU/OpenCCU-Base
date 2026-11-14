#
#
#
#	F. Werner, 14.07.2008
#

ROMFSINST = /usr/bin/romfs-inst.sh

all:

install: subdirs
	./check_CheckOnlyFiles.tcl
	./create_webui_js.tcl
	./utf82ansi.py webui.js
	cp webui.js $(ROMFSDIR)/www/webui/webui.js
	chmod 755 $(ROMFSDIR)/www/webui/webui.js
	./create_style_css.tcl
	./utf82ansi.py style.css
	cp style.css $(ROMFSDIR)/www/webui/style.css
	chmod 755 $(ROMFSDIR)/www/webui/style.css
	./create_devdb_tcl.tcl
	./utf82ansi.py DEVDB.tcl
	cp DEVDB.tcl $(ROMFSDIR)/www/config/devdescr/DEVDB.tcl
	chmod 755 $(ROMFSDIR)/www/config/devdescr/DEVDB.tcl

clean: subdirs_clean

subdirs: $(patsubst %, subdir_%, $(SUBDIRS))

subdirs_install: $(patsubst %, subdir_%_install, $(SUBDIRS))

subdirs_clean: $(patsubst %, subdir_%_clean, $(SUBDIRS))
	rm -f webui.js
	rm -f webui_js.tmp
	rm -f style.css
	rm -f style_css.tmp
	rm -f devdescr.js
	rm -f DEVDB_tcl.tmp
	rm -f DEVDB.tcl
	rm -f st_values.js

$(patsubst %, subdir_%, $(SUBDIRS)) :
	make -C $(patsubst subdir_%, %, $@)

$(patsubst %, subdir_%_install, $(SUBDIRS)) :
	make -C $(patsubst subdir_%_install, %, $@) install

$(patsubst %, subdir_%_clean, $(SUBDIRS)) :
	make -C $(patsubst subdir_%_clean, %, $@) clean

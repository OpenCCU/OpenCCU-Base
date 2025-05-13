#
#	Makefile -- Build instructions for ATMEL/AT91
#
#	F. Werner, 14.07.2008
#

ROMFSINST = /usr/bin/romfs-inst.sh

SUBDIRS = www_source

all: subdirs

install: preinstall subdirs_install
	$(ROMFSINST) -s /var/EULA.de 		       		/www/rega/EULA.de
	$(ROMFSINST) -s /var/EULA.en		       		/www/rega/EULA.en
	$(ROMFSINST) -s /var/EULA.tr 		       		/www/rega/EULA.tr
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/extern/cgi.tcl        /www/cgi.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/common.tcl    /www/common.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/session.tcl   /www/session.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/once.tcl      /www/once.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/user.tcl      /www/user.tcl

	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/extern/cgi.tcl        /www/config/cgi.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/common.tcl    /www/config/common.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/session.tcl   /www/config/session.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/once.tcl      /www/config/once.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/user.tcl      /www/config/user.tcl

	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/extern/cgi.tcl        /www/config/display/cgi.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/once.tcl      /www/config/display/once.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/common.tcl    /www/config/display/common.tcl  

#	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/extern/cgi.tcl            /www/pda/extern/cgi.tcl
#	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/extern/tmpl_parser.tcl    /www/pda/extern/tmpl_parser.tcl
#	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3/rega.tcl              /www/pda/eq3/rega.tcl
#	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3/session.tcl           /www/pda/eq3/session.tcl

	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3/rega.tcl              /www/api/eq3/rega.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3/session.tcl           /www/api/eq3/session.tcl

	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/extern/cgi.tcl       /www/tools/cgi.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/once.tcl     /www/tools/once.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/session.tcl  /www/tools/session.tcl
	$(ROMFSINST) -s ${HM_ROOT}/www/tcl/eq3_old/common.tcl   /www/tools/common.tcl

	$(ROMFSINST) -s ${HM_ROOT}/etc/config/addons/www   /www/addons
	
	chmod -R 755 $(ROMFSDIR)/www || true

preinstall: 
	mkdir -p $(ROMFSDIR)/www
	cd www ;\
	find . -path '*/.svn*' -o -path '*/vssver.scc' -o -path '*/debug.htm' -prune -o -type d -exec mkdir -p $(ROMFSDIR)/www/\{\} \; -o -type f -exec cp \{\} $(ROMFSDIR)/www/\{\} \;

clean: subdirs_clean

subdirs: $(patsubst %, subdir_%, $(SUBDIRS))

subdirs_install: $(patsubst %, subdir_%_install, $(SUBDIRS))

subdirs_clean: $(patsubst %, subdir_%_clean, $(SUBDIRS))

$(patsubst %, subdir_%, $(SUBDIRS)) :
	make -C $(patsubst subdir_%, %, $@)

$(patsubst %, subdir_%_install, $(SUBDIRS)) :
	make -C $(patsubst subdir_%_install, %, $@) install

$(patsubst %, subdir_%_clean, $(SUBDIRS)) :
	make -C $(patsubst subdir_%_clean, %, $@) clean

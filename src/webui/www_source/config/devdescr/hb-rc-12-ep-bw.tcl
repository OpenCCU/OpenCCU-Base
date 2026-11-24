#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-RC-12-EP-BW"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "RemoteControl with ePaper b/w"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-rc-12-ep-bw_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-rc-12-ep-bw.png"]

set P ""

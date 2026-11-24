#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-DIS-EP-75BW"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "battery powered ePaper Display 7.5 b/w"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-dis-ep-75bw_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-dis-ep-75bw.png"]

set P ""

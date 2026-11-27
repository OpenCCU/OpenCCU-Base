#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-DIS-EP-42BW-MAINS"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "mains powered ePaper Display 4.2 b/w"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-dis-ep-42bw-mains_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-dis-ep-42bw-mains.png"]

set P ""

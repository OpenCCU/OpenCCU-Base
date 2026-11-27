#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-DMX-Master"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Universal Simple DMX Controller (Master)"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-dmx-master_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-dmx-master.png"]

set P ""

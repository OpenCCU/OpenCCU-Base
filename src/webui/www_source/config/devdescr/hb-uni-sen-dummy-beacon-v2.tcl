#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-DUMMY-BEACON-V2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Dummy Beacon Device V2"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-dummy-beacon_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-dummy-beacon.png"]

set P ""

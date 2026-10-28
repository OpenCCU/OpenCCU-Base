#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIPW-DRAP"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIPW-DRAP"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/162_hmipw-drap_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/162_hmipw-drap.png"]

set P ""

lappend P {"1" 1 0.240 0.875 0.040}
lappend P {"2" 1 0.450 0.875 0.040}

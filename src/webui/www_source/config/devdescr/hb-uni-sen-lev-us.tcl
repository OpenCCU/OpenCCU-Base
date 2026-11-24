#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-LEV-US"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "universeller Ultraschall F&uuml;llstandsmesser"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-lev-us_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-lev-us.png"]

set P ""

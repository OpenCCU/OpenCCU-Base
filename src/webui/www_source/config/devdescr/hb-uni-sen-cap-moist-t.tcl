#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-CAP-MOIST-T"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "kapazitiver Bodenfeuchtesensor mit Temperaturmessung"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-cap-moist-t_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-cap-moist-t.png"]

set P ""

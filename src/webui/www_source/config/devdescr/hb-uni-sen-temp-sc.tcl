#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-TEMP-SC"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Temperatursensor mit Grenzwert-Schliesserkontakt"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-temp-sc_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-temp-sc.png"]

set P ""

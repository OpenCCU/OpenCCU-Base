#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-TEMP-MAX6675"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "MAX6675 Temperatursensor"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-temp-max6675_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-temp-max6675.png"]

set P ""

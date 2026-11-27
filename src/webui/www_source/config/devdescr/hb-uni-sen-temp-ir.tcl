#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-TEMP-IR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Universal Infrarot Temperatursensor"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-temp-ir_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-temp-ir.png"]

set P ""

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-WEIGHT"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "universeller W&auml;gesensor"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-weight_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-weight.png"]

set P ""

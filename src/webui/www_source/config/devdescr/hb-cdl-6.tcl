#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-CDL-6"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Codelock device"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-cdl-6_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-cdl-6.png"]

set P ""

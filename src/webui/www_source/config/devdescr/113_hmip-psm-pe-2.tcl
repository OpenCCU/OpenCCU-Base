#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-PSM-PE-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PSM-PE"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/215_hmip-psm-pe-2_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/215_hmip-psm-pe-2.png"]

set P ""

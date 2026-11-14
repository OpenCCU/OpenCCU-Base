#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-eTRV-3"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "TRV"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/234_hmip-etrv-3_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/234_hmip-etrv-3.png"]

set P ""


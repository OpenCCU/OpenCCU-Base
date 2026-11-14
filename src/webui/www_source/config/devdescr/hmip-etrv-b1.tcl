#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-eTRV-B1"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "TRV-B"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/178_hmip-etrv-b1_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/178_hmip-etrv-b1.png"]

set P ""

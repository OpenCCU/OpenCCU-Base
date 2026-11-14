#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-eTRV-B-UK-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "TRV-B-UK"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/209_hmip-etrv-b-uk_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/209_hmip-etrv-b-uk.png"]

set P ""

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-PSMCO"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PSMCO"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/235_hmip-psmco_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/235_hmip-psmco.png"]

set P ""
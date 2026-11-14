#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-PDT-PE"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PDT-PE"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/113_hmip-psm-pe_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/113_hmip-psm-pe.png"]

set P ""

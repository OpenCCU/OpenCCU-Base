#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-PSM-CH-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PSM-CH"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/216_hmip-psm-ch-2_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/216_hmip-psm-ch-2.png"]

set P ""

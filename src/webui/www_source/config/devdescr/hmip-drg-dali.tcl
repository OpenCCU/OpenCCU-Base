#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-DRG-DALI"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-DRG-DALI"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/211_hmip-drg-dali_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/211_hmip-drg-dali.png"]

set P ""



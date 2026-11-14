#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Bl1-SM-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Jalousieaktor 1fach Aufputzmontage"
set DESCRIPTION "HM-LC-Bl1-SM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/6_hm-lc-bl1-sm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/6_hm-lc-bl1-sm.png"]


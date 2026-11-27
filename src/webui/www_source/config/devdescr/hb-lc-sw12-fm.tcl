#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Sw12-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "12fach Schaltaktor"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-lc-sw12-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-lc-sw12-fm.png"]

set P ""

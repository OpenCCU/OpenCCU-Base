#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Bl1-Velux"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "battery powered radio-controlled blind actuator 1-channel for Velux"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/7_hm-lc-bl1-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/7_hm-lc-bl1-fm.png"]

set P ""

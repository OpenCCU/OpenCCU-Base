#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Sw1PBU-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-LC-Sw1PBU-FM mit alternativer FW"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/PushButton-2ch-wm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/PushButton-2ch-wm.png"]

set P ""

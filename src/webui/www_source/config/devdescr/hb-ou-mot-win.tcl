#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-MOT-WIN"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Window motor control"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-mot-win_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-mot-win.png"]

set P ""

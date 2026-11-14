#!/bin/tclsh
# This is the CCU-CoPro

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "RPI-RF-MOD"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "RPI-RF-MOD"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/CCU3_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/CCU3.png"]

set P ""

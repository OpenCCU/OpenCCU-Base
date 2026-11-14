#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "VIR-LG-RGB-DIM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "VIR-LG-RGB-DIM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/coupling/hm-coupling-rgb-dim.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/coupling/hm-coupling-rgb-dim.png"]

set P ""


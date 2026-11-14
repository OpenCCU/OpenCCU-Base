#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "VIR-HUE-GTW"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "VIR-HUE-GTW"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/coupling/hm-hue_gateway.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/coupling/hm-hue_gateway.png"]

set P ""


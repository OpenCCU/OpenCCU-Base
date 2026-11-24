#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-IBUT-8"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "iButton Control"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ibut-8_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ibut-8.png"]

set P ""

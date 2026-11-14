#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "RM-110-45/15"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "RM-110-45"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/258_hmip-m-td15_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/258_hmip-m-td15.png"]


set P ""

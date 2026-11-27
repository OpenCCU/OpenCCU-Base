#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-RC-10-POS"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "10 channel RemoteControl"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-rc-x-pos_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-rc-x-pos.png"]

set P ""

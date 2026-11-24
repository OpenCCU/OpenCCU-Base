#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-RC-4-TOUCH"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HB Remote Control 4 Touch"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-rc-4-touch_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-rc-4-touch.png"]

set P ""

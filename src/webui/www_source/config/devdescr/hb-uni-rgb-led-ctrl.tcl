#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-RGB-LED-CTRL"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Adressable RGB LED Controller"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-rgb-led-ctrl_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-rgb-led-ctrl.png"]

set P ""

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-RGB-TEALIGHT-BAT"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "battery powered tealight with WS2812B RGB LED"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-rgb-tealight-bat_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-rgb-tealight-bat.png"]

set P ""

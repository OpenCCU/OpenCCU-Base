#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-RGBW-LED-FX"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "adressable LED FX Controller"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-rgbw-led-fx_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-rgbw-led-fx.png"]

set P ""

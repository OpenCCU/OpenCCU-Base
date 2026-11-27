#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-Sen-TEMP-DS18B20"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Universal 1..8x Temperatursensor DS18B20"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-sen-temp-ds18b20_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-sen-temp-ds18b20.png"]

set P ""

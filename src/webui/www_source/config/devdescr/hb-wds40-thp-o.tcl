#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-WDS40-THP-O"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "sensor with temperature, humidity, air pressure (outdoor)"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/TH_CS_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/TH_CS.png"]

set P ""

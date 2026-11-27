#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-AF-SEN-TEMP"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "AirFlap control with Temperature sensor"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-af-sen-temp_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-af-sen-temp.png"]

set P ""

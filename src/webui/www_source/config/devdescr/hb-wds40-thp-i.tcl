#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-WDS40-THP-I"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "sensor with temperature, humidity, air pressure (indoor)"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/13_hm-ws550sth-i_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/13_hm-ws550sth-i.png"]

set P ""

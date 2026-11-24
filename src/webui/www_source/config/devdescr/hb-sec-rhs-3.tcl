#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-Sec-RHS-3"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HB Rotary Handle Sensor with battery state"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-sec-rhs-3_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-sec-rhs-3.png"]

set P ""

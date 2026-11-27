#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-MOT-FAN"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PWM fan control"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-mot-fan_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-mot-fan.png"]

set P ""

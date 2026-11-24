#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Sw1-TP"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "radio-controlled switch actuator 1-channel, with time profile"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-lc-sw1-tp_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-lc-sw1-tp.png"]

set P ""

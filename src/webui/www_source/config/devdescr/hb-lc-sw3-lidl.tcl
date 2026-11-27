#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Sw3-LIDL"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "radio-controlled switch actuator 3-channel"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-lc-sw3-lidl_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-lc-sw3-lidl.png"]

set P ""

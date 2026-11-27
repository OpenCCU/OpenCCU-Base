#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-EPTR-DPM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "EPEVER Solar Charger and DPM Power Supply"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-eptr-dpm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-eptr-dpm.png"]

set P ""

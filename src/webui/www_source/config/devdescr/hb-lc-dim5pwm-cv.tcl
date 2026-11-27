#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Dim5PWM-CV"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "5-fach LED PWM Dimmer"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/114_hm-lc-dim1t-fm-lf_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/114_hm-lc-dim1t-fm-lf.png"]

set P ""

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sys-sRP-Pl"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------

# set DESCRIPTION "Repeater Funk- Zwischenstecker"
set DESCRIPTION "HM-Sys-sRP-Pl"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/OM55_DimmerSwitch_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/OM55_DimmerSwitch.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""


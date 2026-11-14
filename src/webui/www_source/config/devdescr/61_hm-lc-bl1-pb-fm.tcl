#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Bl1-PB-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Jalousieaktor Unterputzmontage"
set DESCRIPTION "HM-LC-Bl1-PB-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/61_hm-lc-bl1-pb-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/61_hm-lc-bl1-pb-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 60 y: 78 dx: 51 dy: 103
#lappend P {"1" 2 0.24 0.312 0.204 0.412}
#x: 115 y: 78 dx: 51 dy: 103
#lappend P {"2" 2 0.46 0.312 0.204 0.412}

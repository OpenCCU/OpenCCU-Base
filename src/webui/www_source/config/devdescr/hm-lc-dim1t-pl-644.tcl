#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Dim1T-Pl-644"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Zwischenstecker- Dimmaktor 1fach Phasenabschnitt"
set DESCRIPTION "HM-LC-Dim1T-Pl"

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
#x: 137 y: 117 dx: 18 dy: 13
lappend P {"1_part1" 2 0.548 0.468 0.072 0.052}
#x: 153 y: 113 dx: 7 dy: 14
lappend P {"1_part2" 2 0.612 0.452 0.028 0.056}
#Set of 2 forms
lappend P {"1" 5   '1_part1'  '1_part2'  }

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-PB-2-WM55-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Wandtaster 2-fach im 55er Rahmen"
set DESCRIPTION "HM-PB-2-WM55"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/75_hm-pb-2-wm55_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/75_hm-pb-2-wm55.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 61 y: 78 dx: 107 dy: 42
lappend P {"2" 2 0.204 0.23 0.546 0.128}
#x: 61 y: 140 dx: 107 dy: 42
lappend P {"1" 2 0.204 0.65 0.546 0.128}
#x: 61 y: 77 dx: 107 dy: 104
#lappend P {"1+2" 2 0.244 0.308 0.428 0.416}

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-LC-Sw2-DR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HMW-LC-Sw2-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/26_hmw-lc-sw2-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/26_hmw-lc-sw2-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#Kanal 0
#x: 112 y: 191 dx: 12 dy: 16
lappend P {"1" 2 0.448 0.764 0.048 0.064}
#Kanal 1
#x: 124 y: 191 dx: 13 dy: 17
lappend P {"2" 2 0.496 0.764 0.052 0.068}
#Kanal 2
#x: 58 y: 96 dx: 26 dy: 17
lappend P {"3" 2 0.232 0.384 0.104 0.068}
#Kanal 3
#x: 112 y: 96 dx: 26 dy: 17
lappend P {"4" 2 0.448 0.384 0.104 0.068}

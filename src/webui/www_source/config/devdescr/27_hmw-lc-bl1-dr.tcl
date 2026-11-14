#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-LC-Bl1-DR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HMW-LC-Bl1-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/27_hmw-lc-bl1-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/27_hmw-lc-bl1-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 113 y: 193 dx: 11 dy: 15
lappend P {"1" 2 0.452 0.772 0.044 0.06}
#x: 125 y: 193 dx: 12 dy: 15
lappend P {"2" 2 0.5 0.772 0.048 0.06}
#x: 113 y: 97 dx: 24 dy: 15
lappend P {"3" 2 0.452 0.388 0.096 0.06}

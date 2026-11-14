#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-LC-Dim1L-DR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HMW-LC-Dim1L-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/28_hmw-lc-dim1l-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/28_hmw-lc-dim1l-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 78 y: 189 dx: 14 dy: 15
lappend P {"1" 2 0.312 0.756 0.056 0.06}
#x: 92 y: 188 dx: 12 dy: 17
lappend P {"2" 2 0.368 0.752 0.048 0.068}
#x: 92 y: 97 dx: 12 dy: 16
lappend P {"3" 2 0.368 0.388 0.048 0.064}

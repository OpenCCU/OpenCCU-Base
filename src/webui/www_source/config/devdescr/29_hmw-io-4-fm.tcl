#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-IO-4-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HMW-IO-4-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/29_hmw-io-4-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/29_hmw-io-4-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x1: 154 y1: 184 x2: 153 y2: 209 stroke: 5
lappend P {"1" 6 0.616 0.736 0.612 0.836 0.02}
#x1: 168 y1: 184 x2: 167 y2: 209 stroke: 5
lappend P {"2" 6 0.672 0.736 0.668 0.836 0.02}
#x1: 181 y1: 184 x2: 181 y2: 209 stroke: 5
lappend P {"3" 6 0.724 0.736 0.724 0.836 0.02}
#x1: 195 y1: 184 x2: 195 y2: 209 stroke: 5
lappend P {"4" 6 0.78 0.736 0.78 0.836 0.02}

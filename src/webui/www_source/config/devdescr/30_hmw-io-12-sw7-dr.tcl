#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-IO-12-Sw7-DR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "I/O-Modul 12 Eingänge 7 Schaltausgänge Hutschienenmontage (drahtgebunden)"
set DESCRIPTION "HMW-IO-12-Sw7-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/30_hmw-io-12-sw7-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/30_hmw-io-12-sw7-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 66 y: 175 dx: 15 dy: 15
lappend P {"1" 2 0.264 0.7 0.06 0.06}
#x: 82 y: 175 dx: 15 dy: 15
lappend P {"2" 2 0.328 0.7 0.06 0.06}
#x: 115 y: 175 dx: 15 dy: 15
lappend P {"3" 2 0.46 0.7 0.06 0.06}
#x: 131 y: 175 dx: 15 dy: 15
lappend P {"4" 2 0.524 0.7 0.06 0.06}
#x: 162 y: 175 dx: 15 dy: 15
lappend P {"5" 2 0.648 0.7 0.06 0.06}
#x: 177 y: 175 dx: 15 dy: 15
lappend P {"6" 2 0.708 0.7 0.06 0.06}
#x: 82 y: 191 dx: 15 dy: 15
lappend P {"8" 2 0.328 0.764 0.06 0.06}
#x: 66 y: 191 dx: 15 dy: 15
lappend P {"7" 2 0.264 0.764 0.06 0.06}
#x: 115 y: 191 dx: 15 dy: 15
lappend P {"9" 2 0.46 0.764 0.06 0.06}
#x: 131 y: 191 dx: 15 dy: 15
lappend P {"10" 2 0.524 0.764 0.06 0.06}
#x: 161 y: 191 dx: 15 dy: 15
lappend P {"11" 2 0.644 0.764 0.06 0.06}
#x: 177 y: 191 dx: 15 dy: 15
lappend P {"12" 2 0.708 0.764 0.06 0.06}
#x: 67 y: 99 dx: 30 dy: 15
lappend P {"13" 2 0.268 0.396 0.12 0.06}
#x: 115 y: 99 dx: 30 dy: 15
lappend P {"14" 2 0.46 0.396 0.12 0.06}
#x: 162 y: 99 dx: 30 dy: 15
lappend P {"15" 2 0.648 0.396 0.12 0.06}
#x: 19 y: 115 dx: 30 dy: 15
lappend P {"16" 2 0.076 0.46 0.12 0.06}
#x: 66 y: 115 dx: 30 dy: 15
lappend P {"17" 2 0.264 0.46 0.12 0.06}
#x: 115 y: 115 dx: 30 dy: 15
lappend P {"18" 2 0.46 0.46 0.12 0.06}
#x: 162 y: 115 dx: 30 dy: 15
lappend P {"19" 2 0.648 0.46 0.12 0.06}

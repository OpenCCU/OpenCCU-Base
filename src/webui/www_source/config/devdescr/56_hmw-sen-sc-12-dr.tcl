#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-Sen-SC-12-DR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Schließerkontakt-Sensor Hutschienenmontage (drahtgebunden)"
set DESCRIPTION "HMW-Sen-SC-12-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/56_hmw-sen-sc-12-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/56_hmw-sen-sc-12-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
lappend P {"1" 2 0.244 0.688 0.06 0.06}
lappend P {"2" 2 0.304 0.688 0.06 0.06}
lappend P {"3" 2 0.436 0.688 0.06 0.06}
lappend P {"4" 2 0.496 0.688 0.06 0.06}
lappend P {"5" 2 0.62 0.688 0.06 0.06}
lappend P {"6" 2 0.68 0.688 0.06 0.06}
lappend P {"7" 2 0.244 0.752 0.06 0.06}
lappend P {"8" 2 0.304 0.752 0.06 0.06}
lappend P {"9" 2 0.436 0.752 0.06 0.06}
lappend P {"10" 2 0.496 0.752 0.06 0.06}
lappend P {"11" 2 0.62 0.752 0.06 0.06}
lappend P {"12" 2 0.68 0.752 0.06 0.06}

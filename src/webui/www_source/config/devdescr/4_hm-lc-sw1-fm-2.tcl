#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw1-FM-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Schaltaktor 1fach Unterputzmontage"
set DESCRIPTION "HM-LC-Sw1-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/4_hm-lc-sw1-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/4_hm-lc-sw1-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#Ausgang
#x: 72 y: 165 dx: 17 dy: 38
lappend P {"1_AUS" 2 0.288 0.66 0.068 0.152}
#Eingang
#x: 137 y: 165 dx: 17 dy: 38
lappend P {"1_EIN" 2 0.548 0.66 0.068 0.152}
#Ein- und Ausgang
#Set of 2 forms
lappend P {"1" 5   '1_AUS'  '1_EIN'  }

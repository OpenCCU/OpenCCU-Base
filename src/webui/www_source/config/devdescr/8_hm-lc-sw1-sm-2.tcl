#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw1-SM-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Schaltaktor 1fach Aufputzmontage"
set DESCRIPTION "HM-LC-Sw1-SM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/8_hm-lc-sw1-sm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/8_hm-lc-sw1-sm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle
#lappend P {"1+2" 2 187 202 380 302}

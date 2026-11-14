#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw1-Pl-DN-R3"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Zwischenstecker- Schaltaktor 1fach"
set DESCRIPTION "HM-LC-Sw1-Pl-DN-R3"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/107_hm-es-pmsw1-pl-R3_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/107_hm-es-pmsw1-pl-R3.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""


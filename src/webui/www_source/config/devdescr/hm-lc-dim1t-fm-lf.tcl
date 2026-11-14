#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Dim1T-FM-LF"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Dimmaktor 1fach Phasenabschnitt Unterputzmontage"
set DESCRIPTION "HM-LC-Dim1T-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/114_hm-lc-dim1t-fm-lf_thumb_3.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/114_hm-lc-dim1t-fm-lf_3.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

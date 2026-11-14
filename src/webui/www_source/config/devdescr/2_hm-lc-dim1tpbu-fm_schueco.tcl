#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
#set TYPE "HM-LC-Dim1TPBU-FM"
set TYPE "263 133"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk-Dimmaktor 1fach für Markenschalter Unterputz"
set DESCRIPTION "263_133"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/PushButton-2ch-wm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/PushButton-2ch-wm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle
#lappend P {"1+2" 2 187 202 380 302}

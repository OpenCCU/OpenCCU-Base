#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-WS550ST-IO"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Temperatursensor außen"
set DESCRIPTION "HM-WS550ST-IO"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/IP65_G201_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/IP65_G201.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle
#lappend P {"1+2" 2 187 202 380 302}

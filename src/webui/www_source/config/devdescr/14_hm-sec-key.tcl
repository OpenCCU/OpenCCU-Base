#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sec-Key"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "KeyMatic"
set DESCRIPTION "HM-Sec-Key"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/14_hm-sec-key_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/14_hm-sec-key.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle
#lappend P {"1+2" 2 187 202 380 302}

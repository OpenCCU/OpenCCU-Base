#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-WRC2-A"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-WRC2"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/112_hmip-wrc2_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/112_hmip-wrc2.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""

#Taste 2
#x: 61 y: 93 dx: 10 dy: 11
lappend P {"2" 4 0.540 0.366 0.04 0.044}
#Taste 1
#x: 82 y: 93 dx: 10 dy: 11
lappend P {"1" 4 0.540 0.622 0.04 0.044}
#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-WRC6-A"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "WRC6"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/131_hmip-wrc6_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/131_hmip-wrc6.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""

lappend P {"1" 1 0.270 0.310 0.050}
lappend P {"2" 1 0.675 0.285 0.050}

lappend P {"3" 1 0.270 0.510 0.050}
lappend P {"4" 1 0.675 0.480 0.050}

lappend P {"5" 1 0.270 0.706 0.050}
lappend P {"6" 1 0.675 0.671 0.050}

lappend P {"1+2" 5 '1' '2'}
lappend P {"3+4" 5 '3' '4'}
lappend P {"5+6" 5 '5' '6'}

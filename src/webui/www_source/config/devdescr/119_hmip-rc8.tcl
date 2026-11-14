#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-RC8"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "RC8"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/119_hmip-rc8_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/119_hmip-rc8.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""

lappend P {"1" 1 0.472 0.218 0.025}
lappend P {"2" 1 0.600 0.224 0.025}

lappend P {"3" 1 0.476 0.304 0.025}
lappend P {"4" 1 0.606 0.306 0.025}

lappend P {"5" 1 0.480 0.384 0.025}
lappend P {"6" 1 0.610 0.386 0.025}

lappend P {"7" 1 0.484 0.464 0.025}
lappend P {"8" 1 0.614 0.466 0.025}

lappend P {"1+2" 5 '1' '2'}
lappend P {"3+4" 5 '3' '4'}
lappend P {"5+6" 5 '5' '6'}
lappend P {"7+8" 5 '7' '8'}

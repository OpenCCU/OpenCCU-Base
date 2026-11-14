#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "ALPHA-IP-RBG"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "ALPHA-IP-RBG"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50  "/config/img/devices/50/140_alpha-ip-rgb_thumb.png"]
lappend PATHLIST [list  250  "/config/img/devices/250/140_alpha-ip-rgb.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}

set P ""
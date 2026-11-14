#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-BS2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-BS2"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/PushButton-2ch-wm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/PushButton-2ch-wm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}

set P ""
#x: 61 y: 78 dx: 107 dy: 42
lappend P {"1" 2 0.244 0.312 0.428 0.168}
#x: 61 y: 140 dx: 107 dy: 42
lappend P {"2" 2 0.244 0.56 0.428 0.168}
#x: 61 y: 77 dx: 107 dy: 104
lappend P {"1+2" 2 0.244 0.308 0.428 0.416}
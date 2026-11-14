#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "atent"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "DORMA Funk- Handsender zum Bedienen des DORMA Türöffners"
set DESCRIPTION "atent"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/73_hm-atent_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/73_hm-atent.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

#Taste 1
#x: 63 y: 50 dx: 40 dy: 45
lappend P {"1" 4 0.177 0.216 0.166 0.166}
#Taste 2
#x: 123 y: 50 dx: 40 dy: 45
lappend P {"2" 4 0.438 0.216 0.166 0.166}
#Taste 3
#x: 85 y: 121 dx: 57 dy: 63
lappend P {"3" 4 0.273 0.49 0.24 0.235}

#Tastenpaar 1
lappend P {"1+2" 5   '1'  '2' }

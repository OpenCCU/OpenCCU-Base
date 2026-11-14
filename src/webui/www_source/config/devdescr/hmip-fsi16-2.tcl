#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-FSI16-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------

set DESCRIPTION "HmIP-FSI16"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/203_hmip-fsi16_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/203_hmip-fsi16.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}

set P ""





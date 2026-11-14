#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-SWO-PR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-SWO-PR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/169_hmip-swo-pr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/169_hmip-swo-pr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""


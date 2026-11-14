#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-4-3-D"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Handsender 4 Tasten"
set DESCRIPTION "HM-RC-4"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/84_hm-rc-4-x_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/116_hm-rc-4-3_brc-h3.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""


#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sen-RD-O"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "HomeMatic Regensensor"
set DESCRIPTION "HM-Sen-RD-O"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/87_hm-sen-rd-o_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/87_hm-sen-rd-o.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""

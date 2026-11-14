#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-Dis-H-x-EU"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-RC-Dis-H-x-EU"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""

lappend PATHLIST [list  50	"/config/img/devices/50/108_hm-rc-dis-h-x-eu_thump.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/108_hm-rc-dis-h-x-eu.png"]


#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

set P ""


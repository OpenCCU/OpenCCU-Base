#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sec-SD-2-Team"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Rauchmelder (Gruppe)"
set DESCRIPTION "HM-Sec-SD-Team"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/105_hm-sec-sd-2-team_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/105_hm-sec-sd-2-team.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

set P ""

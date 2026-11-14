#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "263 167 Gruppe"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Rauchmelder (Gruppe)"
set DESCRIPTION "263_167_Gruppe"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/52_hm-sec-sd-team_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/52_hm-sec-sd-team.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#set P ""

#lappend P {"Icon" 3 0.092 0.6 'Icon_folgt' 0.14 'verdana' Font.BOLD}

#x: 110 y: 58 fontsize: 45
#lappend P {"1_channel" 3 0.44 0.232 '1' 0.18 'verdana' Font.BOLD}
#lappend P {"1" 5 '1_channel' 'Icon'}


##lappend P {"2_channel" 3 0.44 0.232 '2' 0.18 'verdana' Font.BOLD}
##lappend P {"2" 5 '2_channel' 'Icon'}

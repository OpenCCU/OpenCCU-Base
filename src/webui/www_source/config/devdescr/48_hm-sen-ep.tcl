#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sen-EP"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Sensor für elektrische Impulse"
set DESCRIPTION "HM-Sen-EP"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/48_hm-sen-ep_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/48_hm-sen-ep.png"]
#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

set P ""


#x: 35 y: 153 dx: 19 dy: 11
lappend P {"1_rect" 2 0.14 0.612 0.086 0.05}
lappend P {"2_rect" 2 0.14 0.740 0.086 0.05}

#x: 110 y: 58 fontsize: 45
lappend P {"1_channel" 3 0.44 0.232 '1' 0.18 'verdana' Font.BOLD}
lappend P {"1" 5 '1_channel' '1_rect'}


lappend P {"2_channel" 3 0.44 0.232 '2' 0.18 'verdana' Font.BOLD}
lappend P {"2" 5 '2_channel' '2_rect'}

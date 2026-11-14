#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "WS888"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "WS888"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/9_hm-ws550-us_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/9_hm-ws550-us.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""

##Kanalanzeige zentriert im Bildschirm AG
#x: 35 y: 48 dx: 151 dy: 125
#x: 110 y: 50 fontsize: 31
lappend P {"1" 3 0.440 0.200 '1' 0.124 'verdana' Font.BOLD}
lappend P {"2" 3 0.440 0.200 '2' 0.124 'verdana' Font.BOLD}
lappend P {"3" 3 0.440 0.200 '3' 0.124 'verdana' Font.BOLD}
lappend P {"4" 3 0.440 0.200 '4' 0.124 'verdana' Font.BOLD}
lappend P {"5" 3 0.440 0.200 '5' 0.124 'verdana' Font.BOLD}
lappend P {"6" 3 0.440 0.200 '6' 0.124 'verdana' Font.BOLD}
lappend P {"7" 3 0.440 0.200 '7' 0.124 'verdana' Font.BOLD}
lappend P {"8" 3 0.440 0.200 '8' 0.124 'verdana' Font.BOLD}
lappend P {"9" 3 0.440 0.200 '9' 0.124 'verdana' Font.BOLD}

#x: 101.25 y: 50 fontsize: 31
lappend P {"10" 3 0.405 0.200 '10' 0.124 'verdana' Font.BOLD}



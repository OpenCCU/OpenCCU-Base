#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-SCI-3-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Schließer-Kontakt-Interface Unterputzmontage"
set DESCRIPTION "HM-SCI-3-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/67_hm-sci-3-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/67_hm-sci-3-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

lappend P {"1_Key" 3 0.18 0.216 '1' 0.14 'verdana' Font.BOLD}
lappend P {"1_Kreis" 4 0.220 0.480 0.028 0.028}
lappend P {"1" 5 '1_Key' '1_Kreis' }

lappend P {"2_Key" 3 0.18 0.216 '2' 0.14 'verdana' Font.BOLD}
lappend P {"2_Kreis" 4 0.265 0.405 0.028 0.028}
lappend P {"2" 5 '2_Key' '2_Kreis' }

lappend P {"3_Key" 3 0.18 0.216 '3' 0.14 'verdana' Font.BOLD}
lappend P {"3_Kreis" 4 0.310 0.33 0.028 0.028}
lappend P {"3" 5 '3_Key' '3_Kreis' }


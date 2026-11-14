#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-PBI-4-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk-Tasterschnittstelle 4fach Unterputz"
set DESCRIPTION "HM-PBI-4-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/38_hm-pbi-4-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/38_hm-pbi-4-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""

lappend P {"1_Key" 3 0.18 0.216 '1' 0.14 'verdana' Font.BOLD}
lappend P {"1_Kreis" 4 0.265 0.500 0.028 0.028}
lappend P {"1" 5 '1_Key' '1_Kreis' }

lappend P {"2_Key" 3 0.18 0.216 '2' 0.14 'verdana' Font.BOLD}
lappend P {"2_Kreis" 4 0.287 0.465 0.028 0.028}
lappend P {"2" 5 '2_Key' '2_Kreis' }

lappend P {"3_Key" 3 0.18 0.216 '3' 0.14 'verdana' Font.BOLD}
lappend P {"3_Kreis" 4 0.309 0.430 0.028 0.028}
lappend P {"3" 5 '3_Key' '3_Kreis' }

lappend P {"4_Key" 3 0.18 0.216 '4' 0.14 'verdana' Font.BOLD}
lappend P {"4_Kreis" 4 0.331 0.395 0.028 0.028}
lappend P {"4" 5 '4_Key' '4_Kreis' }

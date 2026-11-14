#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sec-SFA-SM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-Sec-SFA-SM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/55_hm-sec-sfa-sm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/55_hm-sec-sfa-sm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""

##
#x: 87 y: 97 dx: 20 dy: 20
lappend P {"1_Taster" 4 0.348 0.388 0.08 0.08}
#x: 93 y: 76 dx: 9 dy: 9
lappend P {"1_Led" 4 0.372 0.304 0.036 0.036}
lappend P {"1" 5 '1_Taster' '1_Led'}

#x: 138 y: 97 dx: 20 dy: 20
lappend P {"2_Taster" 4 0.552 0.388 0.08 0.08}
#x: 144 y: 76 dx: 9 dy: 9
lappend P {"2_Led" 4 0.576 0.304 0.036 0.036}
lappend P {"2" 5 '2_Taster' '2_Led'}
#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw4-Ba-PCB"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-LC-Sw4-Ba-PCB"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/88_hm-lc-sw4-ba-pcb_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/88_hm-lc-sw4-ba-pcb.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


lappend P {"1" 2 0.140 0.704 0.092 0.052}
lappend P {"2" 2 0.328 0.704 0.092 0.052}
lappend P {"3" 2 0.512 0.704 0.092 0.052}
lappend P {"4" 2 0.688 0.704 0.092 0.052}


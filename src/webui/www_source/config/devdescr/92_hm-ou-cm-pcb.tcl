#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-OU-CM-PCB"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-OU-CM-PCB"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/92_hm-ou-cm-pcb_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/92_hm-ou-cm-pcb.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


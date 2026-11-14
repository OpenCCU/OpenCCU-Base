#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw2-DR-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Schaltaktor 2fach Hutschienenmontage"
set DESCRIPTION "HM-LC-Sw2-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
#lappend PATHLIST [list  50	"/config/img/devices/50/68_hm-lc-sw4-dr_thumb.png"]
#lappend PATHLIST [list  250	"/config/img/devices/250/68_hm-lc-sw4-dr.png"]

lappend PATHLIST [list  50	"/config/img/devices/50/69_hm-lc-sw2-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/69_hm-lc-sw2-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""
lappend P {"1" 4 0.095 0.556 0.045 0.04}
lappend P {"2" 4 0.285 0.556 0.045 0.04}

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sen-MDIR-WM55"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-Sen-MDIR-WM55"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/103_hm-sen-mdir-wm55_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/103_hm-sen-mdir-wm55.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""

lappend P {"1" 2 0.192 0.660 0.524 0.12}
lappend P {"2" 2 0.192 0.252 0.524 0.12}

#Tastenpaar 1
lappend P {"1+2" 5   '1'  '2' }



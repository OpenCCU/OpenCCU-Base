#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-Sec3"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Handsender zum scharf/unscharf- schalten der Alarmanlage"
set DESCRIPTION "HM-RC-Sec3"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/22_hm-rc-sec3-b_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/22_hm-rc-sec3-b.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

#Taste 1
#x: 63 y: 50 dx: 40 dy: 44
lappend P {"1" 4 0.252 0.2 0.16 0.176}
#Taste 2
#x: 123 y: 50 dx: 40 dy: 44
lappend P {"2" 4 0.492 0.2 0.16 0.176}
#Taste 3
#x: 85 y: 120 dx: 56 dy: 62
lappend P {"3" 4 0.34 0.48 0.224 0.248}

#Tastenpaar 1
#Set of 3 forms
lappend P {"1+2" 5   '1'  '2' }

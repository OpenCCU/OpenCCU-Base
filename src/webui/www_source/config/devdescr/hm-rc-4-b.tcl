#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-4-B"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Handsender 4 Tasten"
set DESCRIPTION "HM-RC-4-B"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/18_hm-rc-4_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/18_hm-rc-4.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

#Taste 1
#x: 67 y: 59 dx: 40 dy: 41
lappend P {"1" 4 0.268 0.236 0.16 0.164}
#Taste 2
#x: 119 y: 59 dx: 40 dy: 41
lappend P {"2" 4 0.476 0.236 0.16 0.164}
#Taste 3
#x: 67 y: 120 dx: 40 dy: 41
lappend P {"3" 4 0.268 0.48 0.16 0.164}
#Taste 4
#x: 119 y: 120 dx: 40 dy: 41
lappend P {"4" 4 0.476 0.48 0.16 0.164}
#Tastenpaar 1
#Set of 3 forms
lappend P {"1+2" 5   '1'  '2' }
#Tastenpaar 2
#Set of 3 forms
lappend P {"3+4" 5   '3'  '4' }

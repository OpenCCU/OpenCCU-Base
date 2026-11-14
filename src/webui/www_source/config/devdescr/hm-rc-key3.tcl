#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-Key3"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Handsender zum Bedienen der Keymatic"
set DESCRIPTION "HM-RC-Key3"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/23_hm-rc-key3-b_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/23_hm-rc-key3-b.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

#Taste 1
#x: 63 y: 50 dx: 40 dy: 45
lappend P {"1" 4 0.252 0.2 0.16 0.18}
#Taste 2
#x: 123 y: 50 dx: 40 dy: 45
lappend P {"2" 4 0.492 0.2 0.16 0.18}
#Taste 3
#x: 85 y: 121 dx: 57 dy: 63
lappend P {"3" 4 0.34 0.484 0.228 0.252}

#Tastenpaar 1
lappend P {"1+2" 5   '1'  '2' }

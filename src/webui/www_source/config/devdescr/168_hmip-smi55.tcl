#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-SMI55"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-SMI55"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/168_hmip-smi55_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/168_hmip-smi55.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

#Taste 2
#x: 61 y: 93 dx: 10 dy: 11
lappend P {"2" 4 0.540 0.188 0.04 0.044}
#Taste 1
#x: 82 y: 93 dx: 10 dy: 11
lappend P {"1" 4 0.540 0.820 0.04 0.044}
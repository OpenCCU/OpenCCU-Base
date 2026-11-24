#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-UNI-SenAct-8-8-RC-BAT"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "8x Aktor u. 8x Taster (Batteriebetrieb)"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-uni-senact-8-8-bat_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-uni-senact-8-8-bat.png"]

set P ""

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIPW-SMI55"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIPW-SMI55"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/195_hmipw-smi55_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/195_hmipw-smi55.png"]

set P ""

lappend P {"1" 1 0.530 0.820 0.025}
lappend P {"2" 1 0.505 0.210 0.025}

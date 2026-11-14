#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "ELV-SH-PTI2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "ELV-SH-PTI2"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/252_elv-sh-pti2_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/252_elv-sh-pti2.png"]

set P ""



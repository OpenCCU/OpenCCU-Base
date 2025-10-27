#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-FSI6"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------

set DESCRIPTION "HmIP-FSI6"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/257_ELV-SH-FSI_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/257_ELV-SH-FSI.png"]

set P ""





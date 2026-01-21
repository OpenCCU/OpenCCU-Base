#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "ELV-SH-CWD"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "ELV-SH-CWD"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/243_elv-sh-crc_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/243_elv-sh-crc.png"]

set P ""


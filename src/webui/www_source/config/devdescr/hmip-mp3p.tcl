#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-MP3P"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# TODO Add the correct device description
set DESCRIPTION "HmIP-MP3P"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/186_hmip-mp3p_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/186_hmip-mp3p.png"]

set P ""



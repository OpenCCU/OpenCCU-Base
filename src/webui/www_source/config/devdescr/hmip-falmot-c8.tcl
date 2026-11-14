#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-FALMOT-C8"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-FALMOT-C8"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50  "/config/img/devices/50/245_hmip-falmot-c8_thumb.png"]
lappend PATHLIST [list  250  "/config/img/devices/250/245_hmip-falmot-c8.png"]

set P ""
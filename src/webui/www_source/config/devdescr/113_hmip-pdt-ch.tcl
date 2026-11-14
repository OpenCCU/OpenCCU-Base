#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-PDT-CH"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PDT-CH"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/113_hmip-psm-ch_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/113_hmip-psm-ch.png"]

set P ""

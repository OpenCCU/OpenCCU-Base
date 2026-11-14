#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-STH-A 8DU"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "STH"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/146_hmip-sth_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/146_hmip-sth.png"]

set P ""


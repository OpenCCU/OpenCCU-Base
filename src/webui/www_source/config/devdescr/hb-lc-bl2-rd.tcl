#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-LC-Bl2-RD"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HB-LC-Bl2-RD, zwei Rolladenaktoren plus Regensensor"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-lc-bl2-rd_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-lc-bl2-rd.png"]

set P ""

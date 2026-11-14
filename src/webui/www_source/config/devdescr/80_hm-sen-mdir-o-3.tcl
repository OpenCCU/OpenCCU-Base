#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-Sen-MDIR-O-3"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Bewegungsmelder für Aussenmontage"
set DESCRIPTION "HM-Sen-MDIR-O"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/80_hm-sen-mdir-o_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/80_hm-sen-mdir-o.png"]


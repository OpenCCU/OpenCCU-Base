#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "Wired-LAN-Sniffer"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Wired-LAN-Sniffer"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/162_hmipw-drap_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/162_hmipw-drap.png"]

set P ""

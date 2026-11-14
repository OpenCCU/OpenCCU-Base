#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "RF-LAN-Sniffer"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Virtuelle Fernbedienung (drahtlos)"
set DESCRIPTION "RF-LAN-Sniffer"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/CCU2_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/CCU2.png"]

set P ""

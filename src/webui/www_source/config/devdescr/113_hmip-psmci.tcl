#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "ELV-SH-PSMCI"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "PSMCO"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/249_elv-sh-psmci_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/249_elv-sh-psmci.png"]

set P ""
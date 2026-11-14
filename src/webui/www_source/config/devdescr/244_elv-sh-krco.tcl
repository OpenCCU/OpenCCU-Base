#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "ELV-SH-KRCO"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "ELV-SH-KRCO"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/244_elv-sh-krco_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/244_elv-sh-krco.png"]


set P ""



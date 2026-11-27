#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-IR-Rcv-RC"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "IR Receiver Remote Control"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ir-rcv-rc_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ir-rcv-rc.png"]

set P ""

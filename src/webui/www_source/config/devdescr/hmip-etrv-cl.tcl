#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-eTRV-CL"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "TRV-CL"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/224_hmip-etrv-cl_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/224_hmip-etrv-cl.png"]

set P ""

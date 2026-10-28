#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-BSL"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-BSL"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/173_hmip-bsl_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/173_hmip-bsl.png"]

set P ""

lappend P {"1" 1 0.505 0.635 0.050}
lappend P {"2" 1 0.505 0.360 0.050}

lappend P {"8" 1 0.505 0.360 0.050}
lappend P {"9" 1 0.505 0.360 0.050}
lappend P {"10" 1 0.505 0.360 0.050}

lappend P {"12" 1 0.505 0.635 0.050}
lappend P {"13" 1 0.505 0.635 0.050}
lappend P {"14" 1 0.505 0.635 0.050}
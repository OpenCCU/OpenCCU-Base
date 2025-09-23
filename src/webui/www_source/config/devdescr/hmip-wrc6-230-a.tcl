#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-WRC6-230-A"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "WRC6-230"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/131_hmip-wrc6_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/131_hmip-wrc6.png"]


set P ""

lappend P {"1" 1 0.3 0.358 0.025}
lappend P {"2" 1 0.705 0.315 0.025}
lappend P {"3" 1 0.3 0.53 0.025}
lappend P {"4" 1 0.705 0.495 0.025}
lappend P {"5" 1 0.3 0.706 0.025}
lappend P {"6" 1 0.705 0.671 0.025}

lappend P {"12" 1 0.3 0.358 0.025}
lappend P {"13" 1 0.705 0.315 0.025}
lappend P {"14" 1 0.3 0.53 0.025}
lappend P {"15" 1 0.705 0.495 0.025}
lappend P {"16" 1 0.3 0.706 0.025}
lappend P {"17" 1 0.705 0.671 0.025}

lappend P {"18" 5 '1' '2' '3' '4' '5' '6'}
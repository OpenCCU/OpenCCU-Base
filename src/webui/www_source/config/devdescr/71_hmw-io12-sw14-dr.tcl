#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-IO-12-Sw14-DR"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HMW-IO-12-Sw14-DR"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/71_hmw-io-12-sw14-dr_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/71_hmw-io-12-sw14-dr.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
lappend P {"1" 2 0.106 0.398 0.06 0.06}
lappend P {"2" 2 0.230 0.398 0.06 0.06}
lappend P {"3" 2 0.294 0.398 0.06 0.06}
lappend P {"4" 2 0.422 0.398 0.06 0.06}
lappend P {"5" 2 0.482 0.398 0.06 0.06}
lappend P {"6" 2 0.602 0.398 0.06 0.06}

lappend P {"7" 2 0.046 0.458 0.06 0.06}
lappend P {"8" 2 0.106 0.458 0.06 0.06}
lappend P {"9" 2 0.230 0.458 0.06 0.06}
lappend P {"10" 2 0.294 0.458 0.06 0.06}
lappend P {"11" 2 0.422 0.458 0.06 0.06}
lappend P {"12" 2 0.482 0.458 0.06 0.06}
lappend P {"13" 2 0.602 0.458 0.06 0.06}
lappend P {"14" 2 0.666 0.458 0.06 0.06}

lappend P {"15" 2 0.230 0.69 0.06 0.06}
lappend P {"16" 2 0.294 0.69 0.06 0.06}
lappend P {"17" 2 0.422 0.69 0.06 0.06}
lappend P {"18" 2 0.482 0.69 0.06 0.06}
lappend P {"19" 2 0.602 0.69 0.06 0.06}
lappend P {"20" 2 0.666 0.69 0.06 0.06}

lappend P {"21" 2 0.230 0.755 0.06 0.06}
lappend P {"22" 2 0.294 0.755 0.06 0.06}
lappend P {"23" 2 0.422 0.755 0.06 0.06}
lappend P {"24" 2 0.482 0.755 0.06 0.06}
lappend P {"25" 2 0.602 0.755 0.06 0.06}
lappend P {"26" 2 0.666 0.755 0.06 0.06}


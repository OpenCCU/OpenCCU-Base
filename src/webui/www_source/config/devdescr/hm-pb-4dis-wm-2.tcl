#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-PB-4Dis-WM-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Wandtaster 2-fach mit Display"
set DESCRIPTION "HM-PB-4Dis-WM-2"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/70_hm-pb-4dis-wm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/70_hm-pb-4dis-wm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""


#x: 68 y: 58 dx: 104 dy: 53
##lappend P {"2" 2 0.270 0.236 0.416 0.208}
#x: 68 y: 114 dx: 104 dy: 53
##lappend P {"1" 2 0.270 0.456 0.416 0.208}

#x: 51 y: 61 dx: 139 dy: 30
lappend P {"2" 2 0.204 0.244 0.556 0.12}
#x: 51 y: 168 dx: 138 dy: 32
lappend P {"1" 2 0.204 0.68 0.556 0.12}

lappend P {"4" 2 0.204 0.244 0.556 0.12}
lappend P {"3" 2 0.204 0.68 0.556 0.12}

lappend P {"6" 2 0.204 0.244 0.556 0.12}
lappend P {"5" 2 0.204 0.68 0.556 0.12}

lappend P {"8" 2 0.204 0.244 0.556 0.12}
lappend P {"7" 2 0.204 0.68 0.556 0.12}

lappend P {"10" 2 0.204 0.244 0.556 0.12}
lappend P {"9" 2 0.204 0.68 0.556 0.12}

lappend P {"12" 2 0.204 0.244 0.556 0.12}
lappend P {"11" 2 0.204 0.68 0.556 0.12}

lappend P {"14" 2 0.204 0.244 0.556 0.12}
lappend P {"13" 2 0.204 0.68 0.556 0.12}

lappend P {"16" 2 0.204 0.244 0.556 0.12}
lappend P {"15" 2 0.204 0.68 0.556 0.12}

lappend P {"18" 2 0.204 0.244 0.556 0.12}
lappend P {"17" 2 0.204 0.68 0.556 0.12}

lappend P {"20" 2 0.204 0.244 0.556 0.12}
lappend P {"19" 2 0.204 0.68 0.556 0.12}


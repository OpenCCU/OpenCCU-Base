#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw4-SM-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Schaltaktor 4fach Aufputzmontage"
set DESCRIPTION "HM-LC-Sw4-SM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/3_hm-lc-sw4-sm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/3_hm-lc-sw4-sm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

set P ""
#x1: 34 y1: 224 x2: 34 y2: 245 stroke: 3
lappend P {"1_Part1" 6 0.136 0.896 0.136 0.98 0.012}
#x1: 34 y1: 245 x2: 24 y2: 229 stroke: 3
lappend P {"1_Part2" 6 0.136 0.98 0.096 0.916 0.012}
#x1: 34 y1: 245 x2: 44 y2: 229 stroke: 3
lappend P {"1_Part3" 6 0.136 0.98 0.176 0.916 0.012}
#Set of 3 forms
lappend P {"1_Arrow" 5 '1_Part1'  '1_Part2'  '1_Part3' }
#Verschiebung einer Form. offset_x: 41 offset_y: 0
lappend P {"2_Arrow" 7 '1_Arrow' 0.164 0}
#Verschiebung einer Form. offset_x: 82 offset_y: 0
lappend P {"3_Arrow" 7 '1_Arrow' 0.328 0}
#Verschiebung einer Form. offset_x: 123 offset_y: 0
lappend P {"4_Arrow" 7 '1_Arrow' 0.492 0}

#Taste 1
#x: 61 y: 93 dx: 10 dy: 11
lappend P {"1_Key" 4 0.244 0.372 0.04 0.044}
#Taste 2
#x: 82 y: 93 dx: 10 dy: 11
lappend P {"2_Key" 4 0.328 0.372 0.04 0.044}
#Taste 3
#x: 101 y: 93 dx: 10 dy: 11
lappend P {"3_Key" 4 0.404 0.372 0.04 0.044}
#Taste 4
#x: 121 y: 93 dx: 10 dy: 11
lappend P {"4_Key" 4 0.484 0.372 0.04 0.044}

lappend P {"1" 5   '1_Arrow'  '1_Key'  }
lappend P {"2" 5   '2_Arrow'  '2_Key'  }
lappend P {"3" 5   '3_Arrow'  '3_Key'  }
lappend P {"4" 5   '4_Arrow'  '4_Key'  }

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Dim2T-SM-644"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Dimmaktor 2fach Phasenabschnitt Aufputzmontage"
set DESCRIPTION "HM-LC-Dim2T-SM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/64_hm-lc-dim2T-sm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/64_hm-lc-dim2T-sm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#x1: 34 y1: 224 x2: 34 y2: 245 stroke: 3
lappend P {"1_Part1" 6 0.539 0.864 0.539 0.948 0.012}
#x1: 34 y1: 245 x2: 24 y2: 229 stroke: 3
lappend P {"1_Part2" 6 0.539 0.948 0.49 0.884 0.012}
#x1: 34 y1: 245 x2: 44 y2: 229 stroke: 3
lappend P {"1_Part3" 6 0.539 0.948 0.588 0.884 0.012}

#Set of 3 forms
lappend P {"1_Arrow" 5 '1_Part1'  '1_Part2'  '1_Part3' }
#Verschiebung einer Form. offset_x: 41 offset_y: 0
lappend P {"2_Arrow" 7 '1_Arrow' 0.179 0}


#Taste 1
#x: 61 y: 93 dx: 10 dy: 11
lappend P {"1_Key" 4 0.25 0.26 0.04 0.044}
#Taste 2
#x: 82 y: 93 dx: 10 dy: 11
lappend P {"2_Key" 4 0.328 0.26 0.04 0.044}


lappend P {"1" 5   '1_Arrow'  '1_Key'  }
lappend P {"2" 5   '2_Arrow'  '2_Key'  }

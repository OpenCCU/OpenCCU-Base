#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-OU-CF-Pl"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Gong mit Signalleuchte"
set DESCRIPTION "HM-OU-CF-Pl"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/60_hm-ou-cf-pl_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/60_hm-ou-cf-pl.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 32 y: 126 fontsize: 8
# lappend P {"1" 3 0.128 0.504 'L E D' 0.032 'verdana' Font.BOLD}
# lappend P {"2" 3 0.128 0.504 'GONG' 0.032 'verdana' Font.BOLD}

set P ""
lappend P {"Light_circle" 4 0.688 0.224 0.118 0.112}
#x1: 157 y1: 70 x2: 164 y2: 70 stroke: 4
lappend P {"Light_beam_1" 6 0.628 0.28 0.656 0.28 0.016}
#x1: 164 y1: 50 x2: 170 y2: 55 stroke: 4
lappend P {"Light_beam_2" 6 0.656 0.2 0.68 0.22 0.016}
#x1: 185 y1: 42 x2: 185 y2: 49 stroke: 4
lappend P {"Light_beam_3" 6 0.74 0.168 0.74 0.196 0.016}
#x1: 205 y1: 49 x2: 200 y2: 54 stroke: 4
lappend P {"Light_beam_4" 6 0.82 0.196 0.8 0.216 0.016}
#x1: 206 y1: 70 x2: 214 y2: 70 stroke: 4
lappend P {"Light_beam_5" 6 0.824 0.28 0.856 0.28 0.016}
#x1: 170 y1: 85 x2: 166 y2: 90 stroke: 4
lappend P {"Light_beam_6" 6 0.68 0.34 0.664 0.36 0.016}
#x1: 185 y1: 91 x2: 185 y2: 98 stroke: 4
lappend P {"Light_beam_7" 6 0.74 0.364 0.74 0.392 0.016}
#x1: 200 y1: 85 x2: 205 y2: 90 stroke: 4
lappend P {"Light_beam_8" 6 0.8 0.34 0.82 0.36 0.016}

#Set of x forms
lappend P {"1" 5 'Light_circle'  'Light_beam_1'  'Light_beam_2'  'Light_beam_3'  'Light_beam_4'  'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'  }


#x1: 161 y1: 169 x2: 168 y2: 169 stroke: 4
lappend P {"SP_1" 6 0.644 0.676 0.672 0.676 0.016}
#x1: 168 y1: 169 x2: 168 y2: 204 stroke: 4
lappend P {"SP_2" 6 0.672 0.676 0.672 0.816 0.016}
#x1: 161 y1: 204 x2: 168 y2: 204 stroke: 4
lappend P {"SP_3" 6 0.644 0.816 0.672 0.816 0.016}
#x1: 161 y1: 169 x2: 161 y2: 204 stroke: 4
lappend P {"SP_4" 6 0.644 0.676 0.644 0.816 0.016}
#x1: 168 y1: 169 x2: 179 y2: 158 stroke: 4
lappend P {"SP_5" 6 0.672 0.676 0.716 0.632 0.016}
#x1: 179 y1: 158 x2: 179 y2: 215 stroke: 4
lappend P {"SP_6" 6 0.716 0.632 0.716 0.86 0.016}
#x1: 168 y1: 204 x2: 179 y2: 215 stroke: 4
lappend P {"SP_7" 6 0.672 0.816 0.716 0.86 0.016}
#x1: 182 y1: 175 x2: 208 y2: 158 stroke: 4
lappend P {"SP_beam_1" 6 0.75 0.7 0.832 0.632 0.016}
#x1: 182 y1: 187 x2: 208 y2: 187 stroke: 4
lappend P {"SP_beam_2" 6 0.75 0.748 0.832 0.748 0.016}
#x1: 182 y1: 199 x2: 208 y2: 215 stroke: 4
lappend P {"SP_beam_3" 6 0.75 0.796 0.832 0.86 0.016}

#Set of x forms
lappend P {"2" 5   'SP_1'  'SP_2'  'SP_3'  'SP_4'  'SP_5'  'SP_6'  'SP_7' 'SP_beam_1' 'SP_beam_2' 'SP_beam_3' }



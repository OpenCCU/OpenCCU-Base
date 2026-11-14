#!/bin/tclsh

proc getCircle {name type x1 y1 x2 y2} {
  global xd yd
  set p0 [expr $x1 + $xd]
  set p1 [expr $y1 + $yd]
  return [concat $name $type $p0 $p1 $x2 $y2]
}

proc getStroke {name type x1 y1 x2 y2} {
  global xd yd
  set p0 [expr $x1 + $xd]
  set p1 [expr $y1 + $yd]
  set p2 [expr $x2 + $xd]
  set p3 [expr $y2 + $yd]
  set p4 {0.016}
  return [concat $name $type $p0 $p1 $p2 $p3 $p4]
}

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-OU-CFM-TW"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-OU-CFM-TW"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/117_hm-ou-cfm-tw_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/117_hm-ou-cfm-tw.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------

#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}

set P ""

# We use the original parameter of the HM-OU-CFM-Pl (60_hm-ou-cf-pl.tcl)
# and adapt the position of the 2 icons (speaker / led-indicator)
# to the housing of the HM-OU-CFM-TW

set xd 0.120
set yd 0.432

lappend P [getCircle "Light_circle" 4 0.688 0.224 0.118 0.112]
lappend P [getStroke "Light_beam_1" 6 0.628 0.28 0.656 0.28]
lappend P [getStroke "Light_beam_2" 6 0.656 0.2 0.68 0.22]
lappend P [getStroke "Light_beam_3" 6 0.74 0.168 0.74 0.196]
lappend P [getStroke "Light_beam_4" 6 0.82 0.196 0.8 0.216]
lappend P [getStroke "Light_beam_5" 6 0.824 0.28 0.856 0.28]
lappend P [getStroke "Light_beam_6" 6 0.68 0.34 0.664 0.36]
lappend P [getStroke "Light_beam_7" 6 0.74 0.364 0.74 0.392]
lappend P [getStroke "Light_beam_8" 6 0.8 0.34 0.82 0.36]

#Set of x forms
lappend P {"1" 5 'Light_circle'  'Light_beam_1'  'Light_beam_2'  'Light_beam_3'  'Light_beam_4'  'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'  }

set xd 0.120
set yd -0.556

lappend P [getStroke "SP_1" 6 0.644 0.676 0.672 0.676]
lappend P [getStroke "SP_2" 6 0.672 0.676 0.672 0.816]
lappend P [getStroke "SP_3" 6 0.644 0.816 0.672 0.816]
lappend P [getStroke "SP_4" 6 0.644 0.676 0.644 0.816]
lappend P [getStroke "SP_5" 6 0.672 0.676 0.716 0.632]
lappend P [getStroke "SP_6" 6 0.716 0.632 0.716 0.86]
lappend P [getStroke "SP_7" 6 0.672 0.816 0.716 0.86]
lappend P [getStroke "SP_beam_1" 6 0.75 0.7 0.832 0.632]
lappend P [getStroke "SP_beam_2" 6 0.75 0.748 0.832 0.748]
lappend P [getStroke "SP_beam_3" 6 0.75 0.796 0.832 0.86]

#Set of x forms
lappend P {"2" 5   'SP_1'  'SP_2'  'SP_3'  'SP_4'  'SP_5'  'SP_6'  'SP_7' 'SP_beam_1' 'SP_beam_2' 'SP_beam_3' }

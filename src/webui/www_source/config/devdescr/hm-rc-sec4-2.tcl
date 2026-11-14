#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-Sec4-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk-Handsender Sec 4 Tasten"
set DESCRIPTION "HM-RC-Sec4-2"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/86_hm-rc-sec4-2_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/86_hm-rc-sec4-2.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

# The different parts of the arrow
lappend P {"arrow_part1" 6 0.312 0.288 0.416 0.288 0.012}
lappend P {"arrow_part2" 6 0.312 0.288 0.352 0.248 0.012}
lappend P {"arrow_part3" 6 0.312 0.288 0.352 0.328 0.012}

# Build the arrow
lappend P {"1_Arrow" 5 'arrow_part1'  'arrow_part2'  'arrow_part3' }

#Offset of a form. offset_x: x offset_y: y
lappend P {"2_Arrow" 7 '1_Arrow' 0.028 0.156}
lappend P {"3_Arrow" 7 '1_Arrow' 0.028 0.312}
lappend P {"4_Arrow" 7 '1_Arrow' 0.012 0.468}

# The different channels
lappend P {"1" 5   '2_Arrow' }
lappend P {"2" 5   '1_Arrow' }
lappend P {"3" 5   '4_Arrow' }
lappend P {"4" 5   '3_Arrow' }
lappend P {"1+2" 5   '1_Arrow' '2_Arrow'}
lappend P {"3+4" 5   '3_Arrow' '4_Arrow'}

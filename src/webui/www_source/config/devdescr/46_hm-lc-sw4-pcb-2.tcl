#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw4-PCB-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Schaltaktor 4fach Platine"
set DESCRIPTION "HM-LC-Sw4-PCB"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/46_hm-lc-sw4-pcb_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/46_hm-lc-sw4-pcb.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}

#x: 44 y: 195 dx: 17 dy: 16
lappend P {"Channel1" 2 0.176 0.78 0.068 0.064}
lappend P {"Channel2" 2 0.244 0.78 0.068 0.064}
lappend P {"Channel3" 2 0.312 0.78 0.068 0.064}
lappend P {"Channel4" 2 0.38 0.78 0.068 0.064}

#x: 93 y: 72 fontsize: 35
lappend P {"1_val" 3 0.372 0.288 '1' 0.14 'verdana' Font.BOLD}
lappend P {"2_val" 3 0.372 0.288 '2' 0.14 'verdana' Font.BOLD}
lappend P {"3_val" 3 0.372 0.288 '3' 0.14 'verdana' Font.BOLD}
lappend P {"4_val" 3 0.372 0.288 '4' 0.14 'verdana' Font.BOLD}

#x: 128 y: 196 dx: 11 dy: 11
lappend P {"Circle1" 4 0.512 0.784 0.044 0.044}
lappend P {"Circle2" 4 0.570 0.784 0.044 0.044}
lappend P {"Circle3" 4 0.628 0.784 0.044 0.044}
lappend P {"Circle4" 4 0.686 0.784 0.044 0.044}

lappend P {"1" 5   'Channel1'  '1_val' 'Circle1'}
lappend P {"2" 5   'Channel2'  '2_val' 'Circle2'}
lappend P {"3" 5   'Channel3'  '3_val' 'Circle3'} 
lappend P {"4" 5   'Channel4'  '4_val' 'Circle4'}

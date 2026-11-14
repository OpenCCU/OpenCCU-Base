#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw4-WM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Schaltaktor 4fach"
set DESCRIPTION "HM-LC-Sw4-WM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/76_hm-lc-sw4-wm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/76_hm-lc-sw4-wm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}

#x: 35.0 y: 191.5 dx: 17 dy: 12
lappend P {"Channel1" 2 0.208 0.766 0.065 0.060}
lappend P {"Channel2" 2 0.276 0.766 0.065 0.060}
lappend P {"Channel3" 2 0.344 0.766 0.065 0.060}
lappend P {"Channel4" 2 0.412 0.766 0.065 0.060}

#x: 93 y: 72 fontsize: 35
lappend P {"1_val" 3 0.372 0.288 '1' 0.14 'verdana' Font.BOLD}
lappend P {"2_val" 3 0.372 0.288 '2' 0.14 'verdana' Font.BOLD}
lappend P {"3_val" 3 0.372 0.288 '3' 0.14 'verdana' Font.BOLD}
lappend P {"4_val" 3 0.372 0.288 '4' 0.14 'verdana' Font.BOLD}

#x: 133.5 y: 191.5 dx: 11 dy: 11
lappend P {"Circle1" 4 0.534 0.762 0.044 0.044}
lappend P {"Circle2" 4 0.583 0.762 0.044 0.044}
lappend P {"Circle3" 4 0.637 0.762 0.044 0.044}
lappend P {"Circle4" 4 0.693 0.762 0.044 0.044}

lappend P {"1" 5   'Channel1'  '1_val' 'Circle1'}
lappend P {"2" 5   'Channel2'  '2_val' 'Circle2'}
lappend P {"3" 5   'Channel3'  '3_val' 'Circle3'} 
lappend P {"4" 5   'Channel4'  '4_val' 'Circle4'}

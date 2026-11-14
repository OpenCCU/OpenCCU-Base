#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-PB-6-WM55"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk Wandtaster 6-fach"
set DESCRIPTION "HM-PB-6-WM55"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/86_hm-pb-6-wm55_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/86_hm-pb-6-wm55.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""

lappend P {"1" 2 0.164 0.232 0.112 0.156}
lappend P {"2" 2 0.588 0.232 0.112 0.156}
lappend P {"3" 2 0.164 0.428 0.112 0.156}
lappend P {"4" 2 0.588 0.428 0.112 0.156}
lappend P {"5" 2 0.164 0.616 0.112 0.156}
lappend P {"6" 2 0.588 0.616 0.112 0.156}

#Tastenpaar 1
lappend P {"1+2" 5   '1'  '2' }
#Tastenpaar 2
lappend P {"3+4" 5   '3'  '4'  }
#Tastenpaar 3
lappend P {"5+6" 5   '5'  '6'  }


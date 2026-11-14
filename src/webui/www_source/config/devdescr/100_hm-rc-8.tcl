#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-8"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HM-RC-8"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/100_hm-rc-8_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/100_hm-rc-8.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""
#lappend P {"Icon" 3 0.092 0.6 'Icon_folgt' 0.14 'verdana' Font.BOLD}

#x: 110 y: 58 fontsize: 45
# lappend P {"1_channel" 3 0.44 0.232 '1' 0.18 'verdana' Font.BOLD}
# lappend P {"1" 5 '1_channel' 'Icon'}


# lappend P {"2_channel" 3 0.44 0.232 '2' 0.18 'verdana' Font.BOLD}
# lappend P {"2" 5 '2_channel' 'Icon'}

lappend P {"1" 1 0.374 0.192 0.02}
lappend P {"2" 1 0.537 0.248 0.02}

lappend P {"3" 1 0.374 0.284 0.02}
lappend P {"4" 1 0.537 0.340 0.02}

lappend P {"5" 1 0.374 0.378 0.02}
lappend P {"6" 1 0.537 0.434 0.02}

lappend P {"7" 1 0.374 0.470 0.02}
lappend P {"8" 1 0.537 0.526 0.02}

lappend P {"1+2" 5 '1' '2'}
lappend P {"3+4" 5 '3' '4'}
lappend P {"5+6" 5 '5' '6'}
lappend P {"7+8" 5 '7' '8'}
#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-OU-LED16"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "HomeMatic-Statusanzeige LED 16"
set DESCRIPTION "HM-OU-LED16"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/78_hm-ou-led16_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/78_hm-ou-led16.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set     P ""
#Beispiel Circle: varname kreistyp x y r
#lappend P {"4"   1 380 475 170}
#Beispiel Rectangle varname rechtecktyp x y dx dy
#lappend P {"1+2" 2 187 202 380 302}


set P ""

#x channel 1: 38.0 y: 54.5 dx: 14 dy: 12
lappend P {"1" 2 0.152 0.218 0.064 0.056}
lappend P {"2" 2 0.152 0.277 0.064 0.056}
lappend P {"3" 2 0.152 0.336 0.064 0.056}
lappend P {"4" 2 0.152 0.395 0.064 0.056}
lappend P {"5" 2 0.152 0.454 0.064 0.056}
lappend P {"6" 2 0.152 0.513 0.064 0.056}
lappend P {"7" 2 0.152 0.572 0.064 0.056}
lappend P {"8" 2 0.152 0.631 0.064 0.056}

#x channel 9: 182.0 y: 54.5 dx: 14 dy: 12
lappend P {"9" 2 0.728 0.218 0.064 0.056}
lappend P {"10" 2 0.728 0.277 0.064 0.056}
lappend P {"11" 2 0.728 0.336 0.064 0.056}
lappend P {"12" 2 0.728 0.395 0.064 0.056}
lappend P {"13" 2 0.728 0.454 0.064 0.056}
lappend P {"14" 2 0.728 0.513 0.064 0.056}
lappend P {"15" 2 0.728 0.572 0.064 0.056}
lappend P {"16" 2 0.728 0.631 0.064 0.056}

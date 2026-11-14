#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIPW-DRS4"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIPW-DRS4"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/160_hmipw-drs4_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/160_hmipw-drs4.png"]

set P ""

lappend P {"1" 1 0.217 0.090 0.040}
lappend P {"2" 1 0.217 0.090 0.040}
lappend P {"3" 1 0.217 0.090 0.040}
lappend P {"4" 1 0.217 0.090 0.040}

lappend P {"5" 1 0.650 0.075 0.040}
lappend P {"6" 1 0.650 0.075 0.040}
lappend P {"7" 1 0.650 0.075 0.040}
lappend P {"8" 1 0.650 0.075 0.040}

lappend P {"9" 1 0.210 0.910 0.040}
lappend P {"10" 1 0.210 0.910 0.040}
lappend P {"11" 1 0.210 0.910 0.040}
lappend P {"12" 1 0.210 0.910 0.040}

lappend P {"13" 1 0.645 0.900 0.040}
lappend P {"14" 1 0.645 0.900 0.040}
lappend P {"15" 1 0.645 0.900 0.040}
lappend P {"16" 1 0.645 0.900 0.040}

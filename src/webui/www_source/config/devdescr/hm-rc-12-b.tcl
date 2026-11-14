#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-12-B"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Fernbedienung 12 Tasten schwarz"
set DESCRIPTION "HM-RC-12-B"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/19_hm-rc-12_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/19_hm-rc-12.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 63 y: 103 dx: 11 dy: 18
lappend P {"1" 2 0.252 0.412 0.044 0.072}
#x: 63 y: 127 dx: 11 dy: 18
lappend P {"3" 2 0.252 0.508 0.044 0.072}
#x: 63 y: 151 dx: 11 dy: 18
lappend P {"5" 2 0.252 0.604 0.044 0.072}
#x: 63 y: 175 dx: 11 dy: 18
lappend P {"7" 2 0.252 0.7 0.044 0.072}
#x: 63 y: 200 dx: 11 dy: 18
lappend P {"9" 2 0.252 0.8 0.044 0.072}
#x: 119 y: 200 dx: 11 dy: 18
lappend P {"10" 2 0.476 0.8 0.044 0.072}
#x: 119 y: 175 dx: 11 dy: 18
lappend P {"8" 2 0.476 0.7 0.044 0.072}
#x: 119 y: 151 dx: 11 dy: 18
lappend P {"6" 2 0.476 0.604 0.044 0.072}
#x: 119 y: 127 dx: 11 dy: 18
lappend P {"4" 2 0.476 0.508 0.044 0.072}
#x: 119 y: 103 dx: 11 dy: 18
lappend P {"2" 2 0.476 0.412 0.044 0.072}
#x: 155 y: 200 dx: 17 dy: 16
lappend P {"11" 2 0.62 0.8 0.068 0.064}
#x: 155 y: 176 dx: 17 dy: 16
lappend P {"12" 2 0.62 0.704 0.068 0.064}

#Tastenpaar 1
lappend P {"1+2" 5   '1'  '2' }
#Tastenpaar 2
lappend P {"3+4" 5   '3'  '4'  }
#Tastenpaar 3
lappend P {"5+6" 5   '5'  '6'  }
#Tastenpaar 4
lappend P {"7+8" 5   '7'  '8'  }
#Tastenpaar 5
lappend P {"9+10" 5  '9'  '10'  }
#Tastenpaar 6
lappend P {"11+12" 5 '11'  '12'  }

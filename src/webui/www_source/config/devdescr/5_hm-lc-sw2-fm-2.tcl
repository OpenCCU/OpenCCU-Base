#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-LC-Sw2-FM-2"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "Funk- Schaltaktor 2fach Unterputzmontage"
set DESCRIPTION "HM-LC-Sw2-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/5_hm-lc-sw2-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/5_hm-lc-sw2-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#Ausgang 1
#x: 85 y: 165 dx: 17 dy: 37
lappend P {"1_AUS" 2 0.34 0.66 0.068 0.148}
#Eingang 1
#x: 150 y: 165 dx: 17 dy: 37
lappend P {"1_EIN" 2 0.6 0.66 0.068 0.148}
#Ausgang 2
#x: 64 y: 165 dx: 17 dy: 37
lappend P {"2_AUS" 2 0.256 0.66 0.068 0.148}
#Eingang 2
#x: 127 y: 165 dx: 17 dy: 37
lappend P {"2_EIN" 2 0.508 0.66 0.068 0.148}
#Aus-/Eingang 1
#Set of 2 forms
lappend P {"1" 5   '1_AUS'  '1_EIN'  }
#Aus-/Eingang 2
#Set of 2 forms
lappend P {"2" 5   '2_AUS'  '2_EIN'  }

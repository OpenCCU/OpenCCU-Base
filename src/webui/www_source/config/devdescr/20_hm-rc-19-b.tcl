#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HM-RC-19-B"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
# set DESCRIPTION "Funk- Fernbedienung 19 Tasten mit Display"
set DESCRIPTION "HM-RC-19-B"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/20_hm-rc-19_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/20_hm-rc-19.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

#x: 74 y: 86 dx: 9 dy: 13
lappend P {"1" 2 0.296 0.344 0.036 0.052}
#x: 74 y: 104 dx: 9 dy: 13
lappend P {"3" 2 0.296 0.416 0.036 0.052}
#x: 74 y: 122 dx: 9 dy: 13
lappend P {"5" 2 0.296 0.488 0.036 0.052}
#x: 74 y: 140 dx: 9 dy: 13
lappend P {"7" 2 0.296 0.56 0.036 0.052}
#x: 74 y: 157 dx: 9 dy: 13
lappend P {"9" 2 0.296 0.628 0.036 0.052}
#x: 74 y: 176 dx: 9 dy: 12
lappend P {"11" 2 0.296 0.704 0.036 0.048}
#x: 74 y: 193 dx: 9 dy: 13
lappend P {"13" 2 0.296 0.772 0.036 0.052}
#x: 74 y: 211 dx: 9 dy: 13
lappend P {"15" 2 0.296 0.844 0.036 0.052}
#x: 117 y: 86 dx: 9 dy: 13
lappend P {"2" 2 0.468 0.344 0.036 0.052}
#x: 117 y: 104 dx: 9 dy: 13
lappend P {"4" 2 0.468 0.416 0.036 0.052}
#x: 117 y: 122 dx: 9 dy: 13
lappend P {"6" 2 0.468 0.488 0.036 0.052}
#x: 117 y: 140 dx: 9 dy: 13
lappend P {"8" 2 0.468 0.56 0.036 0.052}
#x: 117 y: 157 dx: 9 dy: 13
lappend P {"10" 2 0.468 0.628 0.036 0.052}
#x: 117 y: 176 dx: 9 dy: 12
lappend P {"12" 2 0.468 0.704 0.036 0.048}
#x: 117 y: 193 dx: 9 dy: 13
lappend P {"14" 2 0.468 0.772 0.036 0.052}
#x: 117 y: 211 dx: 9 dy: 13
lappend P {"16" 2 0.468 0.844 0.036 0.052}

#x: 145 y: 210 dx: 11 dy: 14
lappend P {"17" 2 0.58 0.84 0.044 0.056}

#x: 78 y: 47 dx: 42 dy: 22
lappend P {"18" 2 0.312 0.188 0.168 0.088}



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
#Tastenpaar 7
lappend P {"13+14" 5 '13'  '14'  }
#Tastenpaar 8
lappend P {"15+16" 5 '15'  '16'  }

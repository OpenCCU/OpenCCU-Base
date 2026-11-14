#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HMW-IO-12-FM"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
#set DESCRIPTION "12fach I/O-Modul Unterputzmontage (drahtgebunden)"
set DESCRIPTION "HMW-IO-12-FM"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/59_hmw-io-12-fm_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/59_hmw-io-12-fm.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""

lappend P {"1_num" 3 0.744 0.636 '1' 0.164 'verdana' Font.BOLD}
lappend P {"1_line" 6 0.77 0.08 0.860  0.08 0.016}
lappend P {"1" 5 '1_num' '1_line'}

lappend P {"2_num" 3 0.744 0.636 '2' 0.164 'verdana' Font.BOLD}
lappend P {"2_line" 6 0.77 0.136  0.86 0.136 0.016}
lappend P {"2" 5 '2_num' '2_line'}

lappend P {"3_num" 3 0.744 0.636 '3' 0.164 'verdana' Font.BOLD}
lappend P {"3_line" 6 0.77 0.194  0.86 0.194 0.016}
lappend P {"3" 5 '3_num' '3_line'}

lappend P {"4_num" 3 0.744 0.636 '4' 0.164 'verdana' Font.BOLD}
lappend P {"4_line" 6 0.77 0.25  0.86 0.25 0.016}
lappend P {"4" 5 '4_num' '4_line'}

lappend P {"5_num" 3 0.744 0.636 '5' 0.164 'verdana' Font.BOLD}
lappend P {"5_line" 6 0.77 0.308  0.86 0.308 0.016}
lappend P {"5" 5 '5_num' '5_line'}

lappend P {"6_num" 3 0.744 0.636 '6' 0.164 'verdana' Font.BOLD}
lappend P {"6_line" 6 0.77 0.366  0.86 0.366 0.016}
lappend P {"6" 5 '6_num' '6_line'}

lappend P {"7_num" 3 0.744 0.636 '7' 0.164 'verdana' Font.BOLD}
lappend P {"7_line" 6 0.77 0.424  0.86 0.424 0.016}
lappend P {"7" 5 '7_num' '7_line'}

lappend P {"8_num" 3 0.744 0.636 '8' 0.164 'verdana' Font.BOLD}
lappend P {"8_arc" 4 0.370 0.748  0.036  0.036 0.036}
lappend P {"8" 5 '8_num' '8_arc'}

lappend P {"9_num" 3 0.744 0.636 '9' 0.164 'verdana' Font.BOLD}
lappend P {"9_arc" 4 0.3895 0.704  0.036  0.036 0.036}
lappend P {"9" 5 '9_num' '9_arc'}

lappend P {"10_num" 3 0.744 0.636 '10' 0.164 'verdana' Font.BOLD}
lappend P {"10_arc" 4 0.41 0.65  0.035  0.036 0.036}
lappend P {"10" 5 '10_num' '10_arc'}

lappend P {"11_num" 3 0.744 0.636 '11' 0.164 'verdana' Font.BOLD}
lappend P {"11_arc" 4 0.4293 0.612  0.036  0.036 0.036}
lappend P {"11" 5 '11_num' '11_arc'}

lappend P {"12_num" 3 0.744 0.636 '12' 0.164 'verdana' Font.BOLD}
lappend P {"12_arc" 4 0.448 0.564  0.036  0.036 0.036}
lappend P {"12" 5 '12_num' '12_arc'}

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIP-RCV-50"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIP-RCV-50"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/CCU3-1-50_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/CCU3-1-50.png"]

#Koordinaten für Highlight:
#P {<Formname> <Formtyp> <x,y,dx,dy,r je nach Formtyp>}
#-----------------------------------------------------------------------
set P ""
#x: 91 y: 12 dx: 7 dy: 7
lappend P {"RF_1" 4 0.364 0.048 0.028 0.028}
#x1: 100 y1: 13 x2: 136 y2: 1 stroke: 4
lappend P {"RF_2" 6 0.4 0.052 0.544 0.004 0.016}
#x1: 100 y1: 13 x2: 150 y2: 13 stroke: 4
lappend P {"RF_3" 6 0.4 0.052 0.6 0.052 0.016}
#x1: 100 y1: 13 x2: 136 y2: 26 stroke: 4
lappend P {"RF_4" 6 0.4 0.052 0.544 0.104 0.016}
#x1: 42 y1: 13 x2: 86 y2: 13 stroke: 4
lappend P {"RF_5" 6 0.168 0.052 0.344 0.052 0.016}
#x1: 42 y1: 0 x2: 86 y2: 13 stroke: 4
lappend P {"RF_6" 6 0.168 0 0.344 0.052 0.016}
#x1: 42 y1: 26 x2: 86 y2: 13 stroke: 4
lappend P {"RF_7" 6 0.168 0.104 0.344 0.052 0.016}
#Set of 7 forms
lappend P {"RF" 5   'RF_1'  'RF_2'  'RF_3'  'RF_4'  'RF_5'  'RF_6'  'RF_7'  }

lappend P { "S1"  3 0.25 0.57  '1'  0.3 'verdana' Font.BOLD}
lappend P { "S2"  3 0.25 0.57  '2'  0.3 'verdana' Font.BOLD}
lappend P { "S3"  3 0.25 0.57  '3'  0.3 'verdana' Font.BOLD}
lappend P { "S4"  3 0.25 0.57  '4'  0.3 'verdana' Font.BOLD}
lappend P { "S5"  3 0.25 0.57  '5'  0.3 'verdana' Font.BOLD}
lappend P { "S6"  3 0.25 0.57  '6'  0.3 'verdana' Font.BOLD}
lappend P { "S7"  3 0.25 0.57  '7'  0.3 'verdana' Font.BOLD}
lappend P { "S8"  3 0.25 0.57  '8'  0.3 'verdana' Font.BOLD}
lappend P { "S9"  3 0.25 0.57  '9'  0.3 'verdana' Font.BOLD}
lappend P {"S10"  3 0.175 0.57 '10'  0.3 'verdana' Font.BOLD}
lappend P {"S11"  3 0.175 0.57 '11'  0.3 'verdana' Font.BOLD}
lappend P {"S12"  3 0.175 0.57 '12'  0.3 'verdana' Font.BOLD}
lappend P {"S13"  3 0.175 0.57 '13'  0.3 'verdana' Font.BOLD}
lappend P {"S14"  3 0.175 0.57 '14'  0.3 'verdana' Font.BOLD}
lappend P {"S15"  3 0.175 0.57 '15'  0.3 'verdana' Font.BOLD}
lappend P {"S16"  3 0.175 0.57 '16'  0.3 'verdana' Font.BOLD}
lappend P {"S17"  3 0.175 0.57 '17'  0.3 'verdana' Font.BOLD}
lappend P {"S18"  3 0.175 0.57 '18'  0.3 'verdana' Font.BOLD}
lappend P {"S19"  3 0.175 0.57 '19'  0.3 'verdana' Font.BOLD}
lappend P {"S20"  3 0.175 0.57 '20'  0.3 'verdana' Font.BOLD}
lappend P {"S21"  3 0.175 0.57 '21'  0.3 'verdana' Font.BOLD}
lappend P {"S22"  3 0.175 0.57 '22'  0.3 'verdana' Font.BOLD}
lappend P {"S23"  3 0.175 0.57 '23'  0.3 'verdana' Font.BOLD}
lappend P {"S24"  3 0.175 0.57 '24'  0.3 'verdana' Font.BOLD}
lappend P {"S25"  3 0.175 0.57 '25'  0.3 'verdana' Font.BOLD}
lappend P {"S26"  3 0.175 0.57 '26'  0.3 'verdana' Font.BOLD}
lappend P {"S27"  3 0.175 0.57 '27'  0.3 'verdana' Font.BOLD}
lappend P {"S28"  3 0.175 0.57 '28'  0.3 'verdana' Font.BOLD}
lappend P {"S29"  3 0.175 0.57 '29'  0.3 'verdana' Font.BOLD}
lappend P {"S30"  3 0.175 0.57 '30'  0.3 'verdana' Font.BOLD}
lappend P {"S31"  3 0.175 0.57 '31'  0.3 'verdana' Font.BOLD}
lappend P {"S32"  3 0.175 0.57 '32'  0.3 'verdana' Font.BOLD}
lappend P {"S33"  3 0.175 0.57 '33'  0.3 'verdana' Font.BOLD}
lappend P {"S34"  3 0.175 0.57 '34'  0.3 'verdana' Font.BOLD}
lappend P {"S35"  3 0.175 0.57 '35'  0.3 'verdana' Font.BOLD}
lappend P {"S36"  3 0.175 0.57 '36'  0.3 'verdana' Font.BOLD}
lappend P {"S37"  3 0.175 0.57 '37'  0.3 'verdana' Font.BOLD}
lappend P {"S38"  3 0.175 0.57 '38'  0.3 'verdana' Font.BOLD}
lappend P {"S39"  3 0.175 0.57 '39'  0.3 'verdana' Font.BOLD}
lappend P {"S40"  3 0.175 0.57 '40'  0.3 'verdana' Font.BOLD}
lappend P {"S41"  3 0.175 0.57 '41'  0.3 'verdana' Font.BOLD}
lappend P {"S42"  3 0.175 0.57 '42'  0.3 'verdana' Font.BOLD}
lappend P {"S43"  3 0.175 0.57 '43'  0.3 'verdana' Font.BOLD}
lappend P {"S44"  3 0.175 0.57 '44'  0.3 'verdana' Font.BOLD}
lappend P {"S45"  3 0.175 0.57 '45'  0.3 'verdana' Font.BOLD}
lappend P {"S46"  3 0.175 0.57 '46'  0.3 'verdana' Font.BOLD}
lappend P {"S47"  3 0.175 0.57 '47'  0.3 'verdana' Font.BOLD}
lappend P {"S48"  3 0.175 0.57 '48'  0.3 'verdana' Font.BOLD}
lappend P {"S49"  3 0.175 0.57 '49'  0.3 'verdana' Font.BOLD}
lappend P {"S50"  3 0.175 0.57 '50'  0.3 'verdana' Font.BOLD}

# Wenn das Funksymbol (wie bei CCU1) mit angezeigt werden soll, 
# dann  'RF' hinzufuegen, siehe naechste Zeile
#lappend P {"1" 5   'RF'  'S1' }

lappend P {"1" 5 'S1' }
lappend P {"2" 5 'S2' }
lappend P {"3" 5 'S3' }
lappend P {"4" 5 'S4' }
lappend P {"5" 5 'S5' }
lappend P {"6" 5 'S6' }
lappend P {"7" 5 'S7' }
lappend P {"8" 5 'S8' }
lappend P {"9" 5 'S9' }
lappend P {"10" 5 'S10' }
lappend P {"11" 5 'S11' }
lappend P {"12" 5 'S12' }
lappend P {"13" 5 'S13' }
lappend P {"14" 5 'S14' }
lappend P {"15" 5 'S15' }
lappend P {"16" 5 'S16' }
lappend P {"17" 5 'S17' }
lappend P {"18" 5 'S18' }
lappend P {"19" 5 'S19' }
lappend P {"20" 5 'S20' }
lappend P {"21" 5 'S21' }
lappend P {"22" 5 'S22' }
lappend P {"23" 5 'S23' }
lappend P {"24" 5 'S24' }
lappend P {"25" 5 'S25' }
lappend P {"26" 5 'S26' }
lappend P {"27" 5 'S27' }
lappend P {"28" 5 'S28' }
lappend P {"29" 5 'S29' }
lappend P {"30" 5 'S30' }
lappend P {"31" 5 'S31' }
lappend P {"32" 5 'S32' }
lappend P {"33" 5 'S33' }
lappend P {"34" 5 'S34' }
lappend P {"35" 5 'S35' }
lappend P {"36" 5 'S36' }
lappend P {"37" 5 'S37' }
lappend P {"38" 5 'S38' }
lappend P {"39" 5 'S39' }
lappend P {"40" 5 'S40' }
lappend P {"41" 5 'S41' }
lappend P {"42" 5 'S42' }
lappend P {"43" 5 'S43' }
lappend P {"44" 5 'S44' }
lappend P {"45" 5 'S45' }
lappend P {"46" 5 'S46' }
lappend P {"47" 5 'S47' }
lappend P {"48" 5 'S48' }
lappend P {"49" 5 'S49' }
lappend P {"50" 5 'S50' }

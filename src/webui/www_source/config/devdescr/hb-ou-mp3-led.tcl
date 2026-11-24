#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-MP3-LED"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "MP3 Player with adressable LED"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-mp3-led_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-mp3-led.png"]

set P ""
lappend P {"Light_circle" 4 0.808 0.656 0.118 0.112}
lappend P {"Light_beam_1" 6 0.748 0.712 0.776 0.712 0.016}
lappend P {"Light_beam_2" 6 0.776 0.632 0.8 0.652 0.016}
lappend P {"Light_beam_3" 6 0.86 0.6 0.86 0.628 0.016}
lappend P {"Light_beam_4" 6 0.94 0.628 0.92 0.648 0.016}
lappend P {"Light_beam_5" 6 0.944 0.712 0.976 0.712 0.016}
lappend P {"Light_beam_6" 6 0.8 0.772 0.784 0.792 0.016}
lappend P {"Light_beam_7" 6 0.86 0.796 0.86 0.824 0.016}
lappend P {"Light_beam_8" 6 0.92 0.772 0.94 0.792 0.016}
lappend P {"1" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}
lappend P {"SP_1" 6 0.764 0.12 0.792 0.12 0.016}
lappend P {"SP_2" 6 0.792 0.12 0.792 0.26 0.016}
lappend P {"SP_3" 6 0.764 0.26 0.792 0.26 0.016}
lappend P {"SP_4" 6 0.764 0.12 0.764 0.26 0.016}
lappend P {"SP_5" 6 0.792 0.12 0.836 0.076 0.016}
lappend P {"SP_6" 6 0.836 0.076 0.836 0.304 0.016}
lappend P {"SP_7" 6 0.792 0.26 0.836 0.304 0.016}
lappend P {"SP_beam_1" 6 0.87 0.144 0.952 0.076 0.016}
lappend P {"SP_beam_2" 6 0.87 0.192 0.952 0.192 0.016}
lappend P {"SP_beam_3" 6 0.87 0.24 0.952 0.304 0.016}
lappend P {"2" 5 'SP_1' 'SP_2' 'SP_3' 'SP_4' 'SP_5' 'SP_6' 'SP_7' 'SP_beam_1' 'SP_beam_2' 'SP_beam_3'}

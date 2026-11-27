#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-RC-6-PBU-LED"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "6 remote channel push button with led"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-rc-6-pbu-led_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-rc-6-pbu-led.png"]

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
lappend P {"7" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}
lappend P {"8" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}
lappend P {"9" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}
lappend P {"10" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}
lappend P {"11" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}
lappend P {"12" 5 'Light_circle' 'Light_beam_1' 'Light_beam_2' 'Light_beam_3' 'Light_beam_4' 'Light_beam_5' 'Light_beam_6' 'Light_beam_7' 'Light_beam_8'}

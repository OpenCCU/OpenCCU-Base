#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-RC-2-PBU-LED"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "2 remote channel push button with led"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-rc-2-pbu-led_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-rc-2-pbu-led.png"]

set P ""

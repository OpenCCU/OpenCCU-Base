#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-ES-PMSw1-USB"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "homebrew power meter switch actuator 1-channel for USB"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-es-pmsw1-usb_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-es-pmsw1-usb.png"]

set P ""

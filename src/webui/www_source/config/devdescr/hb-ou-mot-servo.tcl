#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-OU-MOT-SERVO"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Servo motor control"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-ou-mot-servo_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-ou-mot-servo.png"]

set P ""

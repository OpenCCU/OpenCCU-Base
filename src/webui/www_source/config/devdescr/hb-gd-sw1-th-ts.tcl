#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-GD-Sw1-TH-TS"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Garagentor-Aktor, Kontaktschnittstelle und Temperatur-/Luftfeuchtemessung"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-gd-sw1-th-ts_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-gd-sw1-th-ts.png"]

set P ""

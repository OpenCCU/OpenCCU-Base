#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-Sw1-TH-SC"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "Schalt-Aktor, Kontaktschnittstelle und Temperatur-/Luftfeuchtemessung"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-sw1-th-sc_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-sw1-th-sc.png"]

set P ""

#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HmIPW-FIO6"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "HmIPW-FIO6"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/165_hmipw-fio6_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/165_hmipw-fio6.png"]

set P ""

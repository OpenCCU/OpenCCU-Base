#!/bin/tclsh

#Typ dieser Beschreibung (Schlüssel)
#-----------------------------------------------------------------------
set TYPE "HB-RC-4-Dis-TH-CRG"

#Beschreibung des Gerätetyps
#-----------------------------------------------------------------------
set DESCRIPTION "RemoteControl with Display and Temperature measurement (LiPo)"

#Pfade zu den Bildern verschiedener Größe
#lappend PATHLIST <Pixellänge maximale Ausdehnung> <Pfad zum Bild>
#-----------------------------------------------------------------------
set     PATHLIST ""
lappend PATHLIST [list  50	"/config/img/devices/50/hb-rc-4-dis-th-crg_thumb.png"]
lappend PATHLIST [list  250	"/config/img/devices/250/hb-rc-4-dis-th-crg.png"]

set P ""

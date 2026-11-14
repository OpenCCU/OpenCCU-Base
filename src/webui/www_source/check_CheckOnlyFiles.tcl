#!/usr/bin/tclsh

##
# @file check_CheckOnlyFiles.tcl
# @brief Prüft mit jsl die Dateien, die in jsCheckOnlyFiles angegeben sind, fügt sie aber NICHT zur webui.js hinzu.
##

encoding system utf-8

set INPUT_FILE "jsCheckOnlyFiles.txt"
set DIRECTORY "./"

# Eingabe lesen
set fd [open $INPUT_FILE r]
set jsFiles [read $fd]
close $fd

# Kommentare entfernen
regsub -all -line -- {\s*#.*} $jsFiles {} jsFiles

# Inhalte der Dateien verketten
set output ""
foreach filename $jsFiles {
  set fullFilename [file join $DIRECTORY $filename]
  puts "\n\nChecking: $fullFilename\n\n"	
  exec -- jsl -conf jsl.conf -process $fullFilename -nologo -nosummary
	
}

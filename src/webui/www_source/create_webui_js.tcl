#!/usr/bin/tclsh

##
# @file create_webui_js.tcl
# @brief Fügt alle JavaScript-Dateien der HomeMatic WebUI zu einer einzelnen
#        zusammen.
##

encoding system utf-8

set OUTPUT_FILE "webui.js"
set INPUT_FILE "jsfiles.txt"
set DIRECTORY "./"

source create_resource_js.tcl
source create_devdescr_js.tcl
source create_st_value_js.tcl


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
	
  puts "fullFilename: $fullFilename"
	
# Prüfen
	#exec -- jsl -conf jsl.conf -process $fullFilename -nologo -nosummary
	
  set fd [open $fullFilename r]
	append output [encoding convertfrom iso8859-1 [read $fd]]
	close $fd
}
# JavaScript komprimieren
#set output [exec -- jsmin << $output]

# temporäre Datei schreiben
set fd [open $OUTPUT_FILE w]
puts $fd $output
close $fd


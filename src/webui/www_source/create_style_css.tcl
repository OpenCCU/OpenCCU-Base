#!/usr/bin/tclsh

##
# @file create_style_css.tcl
# @brief Fügt alle CSS-Dateien der HomeMatic WebUI zu einer zusammen
##

encoding system utf-8

set OUTPUT_FILE "style.css"
set INPUT_FILE "cssfiles.txt"
set DIRECTORY "./"

# Eingabe lesen
set fd [open $INPUT_FILE r]
set cssFiles [read $fd]
close $fd

# Kommentare entfernen
regsub -all -line -- {\s*#.*} $cssFiles {} cssFiles

# Inhalte der Dateien verketten
set output ""
foreach filename $cssFiles {
	set fullFilename [file join $DIRECTORY $filename]
	
	set fd [open $fullFilename r]
	append output [encoding convertfrom iso8859-1 [read $fd]]
	close $fd
}

# temporäre Datei schreiben
set fd [open $OUTPUT_FILE w]
puts $fd $output
close $fd

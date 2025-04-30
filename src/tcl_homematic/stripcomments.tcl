#!/usr/bin/tclsh

# Zeigt die Verwendung des Tools an
proc showUsage { } {
	puts "stripcomment.tcl V 1.0, Copyright (c) 2009 by eQ-3 Enwicklung GmbH"
	puts "Usage"
	puts "\tstripcomment.tcl <file>"
	puts "<file> tcl file"
}

##################
# Einsprungpunkt #
##################

# keine Argumente - Hilfe Anzeigen
if { "" == $argv } then {
	showUsage
	exit 1
}

# Kommentare entfernenman cp
set filename [lindex $argv 0]

set fd [open $filename r]
while { ![eof $fd] } {
	set line [gets $fd]
	
	if {![regexp {\s*#($|[^\!])} $line dummy]} {
		puts $line
	}
	
}

close $fd
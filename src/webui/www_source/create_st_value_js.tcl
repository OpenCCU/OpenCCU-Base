#!/usr/bin/tclsh

#set ST_FILENAME "../../devicetypes/stringtable_de.txt"
set ST_FILENAME "../www/config/stringtable_de.txt"
set JS_FILENAME "../../devicetypes/st_values.js"

set FILENAME "st_values.js"


proc parse_line {zeile p_context p_value} {

	upvar $p_context context
	upvar $p_value   value
	
	set tokenizer [split $zeile "\t"]
	
	set context [lindex $tokenizer 0]
	set value   [lindex $tokenizer 1]
}


set fd [open $FILENAME w]

if { ! [catch {open $ST_FILENAME "r"} stFile] } then {

	puts $fd "elvST = new Array();"
	
	while {! [eof $stFile] } {
		gets $stFile zeile

		if {$zeile == ""} then {
			continue
		}

     		if { [regexp {^([^\t]*)\t+(.*)$} $zeile dummy key value] } then {
      		if { [regexp {^<.*>$} $value] } then {
				#Dieser Parameter wurde noch nicht übersetzt. Nicht ins JavaScript-Array einfü!
				continue
			}
		}

		set context ""
		set value ""
		
		parse_line $zeile context value
		
		puts $fd "elvST\['$context'\] = '$value';"
	}
	
  	catch {close $stfile}
}

if { ! [catch {open $JS_FILENAME "r"} jsfile] } then {

	while {! [eof $jsfile] } {
		gets $jsfile zeile

		#Weiche EOF-Marke:
		if {$zeile == "//<."} then { break }
		
		puts $fd $zeile
	}

	catch {close $jsfile}
}


close $fd

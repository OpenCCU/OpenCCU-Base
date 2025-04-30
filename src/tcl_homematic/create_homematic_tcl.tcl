#!/usr/bin/tclsh

set OUTPUT_FILE "homematic.tcl"
set INPUT_FILE "files.txt"

set fd [open $INPUT_FILE r]
set files [read $fd]
close $fd

set fd [open "|cat $files" r]
set content [read $fd]
close $fd

set fd [open $OUTPUT_FILE w]
puts -nonewline $fd $content
close $fd


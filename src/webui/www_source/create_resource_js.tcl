#!/usr/bin/tclsh

##
#
##

set RESOURCE_FILE "resources.txt"
set JS_FILE "resource.js"


proc loadFromFile { filename } {
  set fd [open $filename r]
  set result [read $fd]
  close $fd
  
  return $result
}

proc stripComments { text } {
  regsub -all -line -- {\s*#.*} $text {} result
  
  return $result
}

proc getResource {type filename} {
  set result ""
  
  switch -exact $type {
    STRING  { set result [getStringResource $filename] }
    default { error "unkown resource type ($type)" }
  }

  return $result
}

proc getStringResource { filename } {
  set content [loadFromFile $filename]
  set content [string map {
    "\r"  "\\r"
    "\n"  "\\n"
    "\t"  "\\t"
    "\f"  "\\f"
    "\b"  "\\b"
    "/"   "\\/"
    "\\"  "\\\\"
    "\""  "\\\""
    "\'"  "\\\'"
  } $content ]
  
  return "\"$content\""
}

set content [loadFromFile $RESOURCE_FILE]
set content [stripComments $content]
set lines   [split $content "\r\n"]

set fd [open $JS_FILE w]
foreach line $lines {
  if { [regexp {\s*(\S+)\s*:\s*(\S+)\s*=\s*(\S+)} $line dummy varname type filename] } {
    puts $fd "$varname = [getResource $type $filename];"
  }
}
close $fd

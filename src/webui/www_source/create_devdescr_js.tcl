#!/usr/bin/tclsh

set DEVDB_DIRECTORY "config/devdescr"
set JS_FILENAME     "config/js/jsDevDescr.js"

set FILENAME "devdescr.js"

set DEV_LIST ""

array set DEV_DESCRIPTION ""
array set DEV_PATHS       ""
array set DEV_HIGHLIGHT   ""

proc AddUIDescription {key} {
  global DEV_LIST
  
  lappend DEV_LIST $key
}

proc AddDescription {key desc} {
  global DEV_DESCRIPTION
  
  set DEV_DESCRIPTION($key) $desc
}

proc AddPaths {key p_PATHS} {
  global DEV_PATHS  
  upvar $p_PATHS PATHS
  
  set DEV_PATHS($key) $PATHS
}

proc AddCoordinates {key p_P} {
  global DEV_HIGHLIGHT
  upvar $p_P P
    
  set DEV_HIGHLIGHT($key) $P
}

proc DEV_getImagePath {key size} {
  global DEV_PATHS DEV_LIST
  
  if { [lsearch $DEV_LIST $key] < 0 } then { return "" }
  
  set pathlist $DEV_PATHS($key)
  set path ""

  foreach px $pathlist {
    
    set asize [lindex $px 0] 
    set apath [lindex $px 1] 

    if {$asize == $size} then {
      set path $apath
      break
    }
  }


  return $path
}

set filelist [lsort [glob -nocomplain [file join "$DEVDB_DIRECTORY/*.tcl"]]]
  
foreach file $filelist {

  if {[file tail $file] == "DEVDB.tcl"} then { continue }

  set TYPE        ""
  set DESCRIPTION ""
  set PATHLIST    ""
  set P           ""
   
  source $file

  catch {
    AddUIDescription $TYPE
    AddDescription   $TYPE $DESCRIPTION
    AddPaths         $TYPE PATHLIST
    AddCoordinates   $TYPE P      
  }
}

set fd [open $FILENAME w]

puts $fd "DEV_LIST        = new Array();"
puts $fd "DEV_DESCRIPTION = new Array();"
puts $fd "DEV_PATHS       = new Array();"
puts $fd "DEV_HIGHLIGHT   = new Array();"

foreach descr $DEV_LIST {
  
  puts $fd "DEV_LIST.push('$descr');"
  puts $fd "DEV_DESCRIPTION\[\"$descr\"\] = \"$DEV_DESCRIPTION($descr)\";"  

  puts $fd "DEV_PATHS\[\"$descr\"\] = new Object();"
  foreach pathentry $DEV_PATHS($descr) {

    set size [lindex $pathentry 0]
    set path [lindex $pathentry 1]
    
    puts $fd "DEV_PATHS\[\"$descr\"\]\[\"$size\"\] = \"$path\";"
  }

  puts $fd "DEV_HIGHLIGHT\[\"$descr\"\] = new Object();"
  foreach coordentry $DEV_HIGHLIGHT($descr) {

    set varname     [lindex $coordentry 0]
    set koordstruct [lrange $coordentry 1 end]

    set js_koord_arr ""
    foreach koord $koordstruct {
      append js_koord_arr "$koord, "
    }
    set js_koord_arr [string trimright $js_koord_arr " ,"]
    
    puts $fd "DEV_HIGHLIGHT\[\"$descr\"\]\[\"$varname\"\] = \[$js_koord_arr\];"
  }
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

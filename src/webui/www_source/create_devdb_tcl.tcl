#!/usr/bin/tclsh

encoding system utf-8

set DEVDB_DIRECTORY "config/devdescr"

set OUTPUT_FILE "DEVDB.tcl"

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

set filelist [glob -nocomplain [file join "$DEVDB_DIRECTORY/*.tcl"]]
  
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

set fd [open $OUTPUT_FILE w]

puts $fd "#!/bin/tclsh"
puts $fd "set DEV_LIST [list $DEV_LIST]"
puts $fd "array set DEV_DESCRIPTION [list [array get DEV_DESCRIPTION]]"
puts $fd "array set DEV_PATHS       [list [array get DEV_PATHS]]"
puts $fd "array set DEV_HIGHLIGHT   [list [array get DEV_HIGHLIGHT]]"

puts $fd {
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
}

close $fd

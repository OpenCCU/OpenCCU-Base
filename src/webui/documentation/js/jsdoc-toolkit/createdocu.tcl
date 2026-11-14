#!/usr/bin/tclsh

set IGNORE_DIRECTORIES {
  ../../../www/webui/js/extern
}

proc listJsFiles { path } {
  global IGNORE_DIRECTORIES
  
  set jsFiles ""
  
  foreach jsFile [glob -nocomplain -directory $path -- *.js] {
    lappend jsFiles $jsFile
  }
  
  foreach filename [glob -nocomplain -directory $path -- *] {
    if { [file isdirectory $filename] } then {
      if { 0 > [lsearch -exact $IGNORE_DIRECTORIES $filename] } then {
        set jsFiles [concat $jsFiles [listJsFiles $filename]]
      }
    }
  }
  
  return $jsFiles
}

proc createConfigFile { filename } {
  set fd [open $filename w]

  puts $fd "/*"
  puts $fd "  This is an example of one way you could set up a configuration file to more"
  puts $fd "  conveniently define some commandline options. You might like to do this if"
  puts $fd "  you frequently reuse the same options. Note that you don't need to define"
  puts $fd "  every option in this file, you can combine a configuration file with"
  puts $fd "  additional options on the commandline if your wish."
  puts $fd "  You would include this configuration file by running JsDoc Toolkit like so:"
  puts $fd "  java -jar jsrun.jar app/run.js',', -c=conf/sample.conf"
  puts $fd "*/"
  puts $fd ""

  puts $fd "{"

  puts $fd "  // source files to use"
  puts $fd "  _: \["
  
  set first 1
  foreach jsFile [listJsFiles ../../../www] {
    if { $first != 1 } then { puts $fd "," } else { set first 0 }
    puts -nonewline $fd "    '$jsFile'"
  }
  
  puts $fd ""
  puts $fd "  ],"
  puts $fd ""

  puts $fd "  // document all functions, even uncommented ones"
  puts $fd "  a: true,"
  puts $fd ""

  puts $fd "  // including those marked @private"
  puts $fd "  p: true,"
  puts $fd ""

  puts $fd "  // use this directory as the output directory"
  puts $fd "  d: \"../docs\","
  puts $fd ""

  puts $fd "  // use this template"
  puts $fd "  t: \"templates/jsdoc\""
  puts $fd ""

  puts $fd "}"

  close $fd
}

if { [file isdirectory ../docs] } then { exec -- rm -r ../docs }
createConfigFile "jsdoc.conf"
exec -- java -jar jsrun.jar app/run.js -c=jsdoc.conf

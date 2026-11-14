#!/usr/bin/tclsh

##
# Entfernt die nachfolgend genannten Elemente aus einem XML-Dokument:
#   - XML Processing Instructions
#   - XML Kommentate
#   - "docu"-Tags
#   - ungenutzten Leerraum
##
proc stripDocu { xml } {
  regsub -all -- {<\?(.*?)\?>} $xml {} xml               ;# entferne XML Processing Instructions
  regsub -all -- {<!--(.*?)-->} $xml {} xml              ;# enferne XML Kommentare 
	regsub -all -- {<docu[^>]*/>} $xml {} xml              ;# entferne 'einzeilige' docu-Tags
  regsub -all -- {<docu(.*?)/docu(.*?)*>} $xml {} xml    ;# entferne 'mehrzeilige' docu-Tags
  regsub -all -- {>\s*?<} $xml "><" xml                  ;# entferne ungenutzten Leerraum
  return [string trim $xml]
}

##
# Lädt die Datei fielName
##
proc loadFile { fileName } {
  set fd -1
  
  catch { set fd [open $fileName r] }
  if { $fd < 0 } then {
    puts "error: can't open file $fileName"
    exit 1
  }
  
  set content [read $fd]
  close  $fd;
  return $content
}

##
# Speichert eine Zeichenkette in einer Datei
##
proc saveToFile { fileName content } { 
  set fd [open $fileName w]
  puts $fd $content
  close $fd
}

##
# Gibt eine Beschreibung des Scripts aus
##
proc writeUsage { } {
  puts "stripdocu.tcl <xmlfile> [<outfile>]"
  puts "  Entfernt alle docu-Tags aus einer Gerätebeschreibungsdatei"
  puts ""
  puts " <xmlfile> XML-Datei"
	puts " <outfile> Ausgabedatei (Optional)"
	puts ""
  exit 1
}

##################
# Einsprungpunkt #
##################

switch -exact -- $argc {
	1       { puts -nonewline [stripDocu [loadFile [lindex $argv 0]]] }
	2       { saveToFile [lindex $argv 1] [stripDocu [loadFile [lindex $argv 0]]] } 
	default { writeUsage }
}

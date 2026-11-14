#!/usr/bin/tclsh

package require tdom

set COMMON_VALUES {INHIBIT=TRUE INHIBIT=FALSE INHIBIT}


proc read_translations {file} {
  global ids COMMON_VALUES
  if [file readable $file] {
    set fd [open $file r]
    while { [gets $fd line] >= 0 } {
      if { [regexp {^([^\t]*)\t+(.*)$} $line dummy key value] } {
		  
		#Nur die Keys, die schon eine Übersetzung haben (keine < > Markierungen am Rand):
      	if { ! [regexp {^<.*>$} $value] } {
        	set ids($key) $value
        	if { [string first "|" $key] < 0 } {
           		lappend COMMON_VALUES $key
        	}
		}
		#===============================================

      }
    }
    close $fd
  }
}

proc write_translations {file} {
  global ids
  set fd [open $file w]
  foreach id [lsort [array names ids]] {
    puts $fd "$id\t$ids($id)"
  }
  close $fd
}

proc add_value {context value} {
  global ids COMMON_VALUES
  if { [lsearch -exact $COMMON_VALUES $value] < 0 && $context != ""} {
    set id "$context|$value"
  } else {
    set id "$value"
  }
  if { ! [info exists ids($id)] } {
    set ids($id) "<$id>"
  }
}

proc do_file {file} {

  set fd [open $file]
  set xml_text [read $fd]
  close $fd
  while 1 {
    set left [string first {<!--} $xml_text]
    if {$left < 0} break
    set right [string first {-->} $xml_text $left]
    if {$right < 0 } {
      puts "Error: unmatched comment in file $file"
      return
    }
    set xml_text [string replace $xml_text $left [expr $right + 2]]
  }

  if { [catch {set xml [dom parse $xml_text]} e ] } then {	  
	puts "<ERROR> error parsing file $file :"
	puts $e
	return	
  }

  if { [catch {set root [$xml documentElement]} e ] } then {
	puts "<ERROR> error getting root element in file $file :"
	puts $e
	return
  }
	
  if { [catch {set channels_node [$root child all channels]} e ] } then {
	puts "<ERROR> error collecting channels in file $file :"
	puts $e
	return
  }
	
  if { [catch {set channel_node_list [$channels_node childNodes]} e ] } then {
	puts "<ERROR> error collecting channel childNodes in file $file :"
	puts $e
	return
  }
  
  set add_param_name 1

  foreach channel_node $channel_node_list {
		
	#Parametersets mit Typ VALUES
   	add_values_by_pstype root channel_node VALUES 0

	#Parametersets mit Typ MASTER
   	add_values_by_pstype root channel_node MASTER $add_param_name
  }

  set ps_dev_master [$root child all paramset]
	
  foreach param_node [$ps_dev_master child all parameter] {
	add_param_node param_node "" MASTER $add_param_name
  }
  
  $xml delete
}

proc add_values_by_pstype {p_root p_channel_node ps_type {add_param_name 0}} {

	upvar $p_root         root
	upvar $p_channel_node channel_node

	
    set channel_type [$channel_node getAttribute type]
	
    if {$channel_type == "MAINTENANCE"} return
		
    set paramset_node [$channel_node child all paramset type $ps_type]
	
    if { [string length $paramset_node] == 0 } return
		
    set subset_node [$paramset_node child all subset]
    if [string length $subset_node] {
      set ref [$subset_node getAttribute ref]
  	  set paramset_node [[$root child all paramset_defs] child all paramset id $ref]
    }

    foreach param_node [$paramset_node child all parameter] {
		add_param_node param_node $channel_type $ps_type $add_param_name
	}
}

proc add_param_node {p_param_node channel_type ps_type {add_param_name 0}} {

	upvar $p_param_node param_node
	

    if [$param_node hasAttribute ui_flags] {
      if { [string first "internal" [$param_node getAttribute ui_flags]] >= 0 } return
    }
    if [$param_node hasAttribute hidden] {
      if { [string first "t" [$param_node getAttribute hidden]] == 0 } return
    }
    set logical_node [$param_node child all logical]
    set value_id [$param_node getAttribute id]
	  
	#puts "$value_id $channel_type"
	  
    set value_type [$logical_node getAttribute type]
	  
	if {$add_param_name == 1} then {
		add_value $channel_type $value_id
	}
	  
    switch $value_type {
      boolean {
			#Achtung: MASTER-Parameter werden im WebUI als Checkbox umgesetzt und brauchen keine Übersetzung!
		  if {$ps_type != "MASTER"} then {
       		add_value $channel_type $value_id=TRUE
       		add_value $channel_type $value_id=FALSE
		  }
      }
      option {
        set options_list [$logical_node child all option]
    	  foreach option_node $options_list {
            set option_id [$option_node getAttribute id]
            add_value $channel_type $value_id=$option_id
        }
      }
      default {
        add_value $channel_type $value_id
      }
    }
    set special_values_list [$logical_node child all special_value]
    foreach special_value_node $special_values_list {
      set special_value_id [$special_value_node getAttribute id]
      add_value $channel_type $value_id=$special_value_id
    }
}

read_translations stringtable_de.txt

foreach arg $argv {
  puts "do file $arg"
  do_file $arg
}

write_translations stringtable_de.txt

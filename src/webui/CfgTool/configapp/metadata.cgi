#!/bin/tclsh
source $env(DOCUMENT_ROOT)/once.tcl
sourceOnce $env(DOCUMENT_ROOT)/cgi.tcl
sourceOnce $env(DOCUMENT_ROOT)/session.tcl
sourceOnce $env(DOCUMENT_ROOT)/common.tcl
loadOnce tclrpc.so

set iface_url $env(BIDCOS_SERVICE)

proc cmd_get_metadata {} {
    global iface_url
    
    import object_id
    import data_id
    
    set value [xmlrpc $iface_url getMetadata [list string $object_id] [list string $data_id]]
    
    puts $value
}

proc cmd_set_metadata {} {
    global iface_url
    
    import id
    import value

    set fields [split $id ";"]
    
    set object_id [lindex $fields 0]
    set data_id [lindex $fields 1]
    
    set type string
    regexp {^\(([a-zA-Z]+)\)(.+)$} $object_id dummy type object_id

    xmlrpc $iface_url setMetadata [list string $object_id] [list string $data_id] [list $type $value]
    puts $value
}

proc cmd_set_bidcos_interface {} {
    global iface_url
    
    import device
    import bidcos_iface
    import roaming

    xmlrpc $iface_url setBidcosInterface [list string $device] [list string $bidcos_iface] [list bool $roaming]
    puts $bidcos_iface
}

proc cmd_set_parameter {} {
    global iface_url TYPE_MAP
    
    import id
    import value

    set fields [split $id ";"]
    
    set object_id [lindex $fields 0]
    set paramset_id [lindex $fields 1]
    set param_id [lindex $fields 2]

    array set ps_descr [xmlrpc $iface_url getParamsetDescription [list string $object_id] [list string $paramset_id]]
    array set param_descr $ps_descr($param_id)
    set type $param_descr(TYPE)

    if { "$paramset_id" == "VALUES" } {
        xmlrpc $iface_url setValue [list string $object_id] [list string $param_id] [list $TYPE_MAP($type) $value]
    } else {
        set struct ""
        set sentry ""
        lappend sentry $param_id
        lappend sentry [list $TYPE_MAP($type) $value]
        lappend struct $sentry
        xmlrpc $iface_url putParamset [list string $object_id] [list string $paramset_id] [list struct $struct]
    }
    puts $value
}

proc cmd_change_array_member {} {
    global iface_url
    
    import id
    import value
    
    set fields [split $id ";"]
    
    set object_id [lindex $fields 0]
    set data_id [lindex $fields 1]
    set index [lindex $fields 2]
    set sub_index -1
    if { [llength $fields] >= 4 } {
        set sub_index [lindex $fields 3]
    }
    
    set arr [xmlrpc $iface_url getMetadata [list string $object_id] [list string $data_id]]
    
    while { [llength $arr] <= $index } { lappend arr "" }
    
    if { $sub_index < 0 } {
        set elem $value
    } else {
        set elem [lindex $arr $index]
        while { [llength $elem] <= $sub_index } { lappend elem "" }
        lset elem $sub_index $value
    }
    
    lset arr $index $value
    
    xmlrpc $iface_url setMetadata [list string $object_id] [list string $data_id] [list array $arr]

}

cgi_eval {
    #cgi_debug -on
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }
    
    http_head

    import cmd
    
    cmd_$cmd

}


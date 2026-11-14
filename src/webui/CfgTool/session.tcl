#!/bin/tclsh
source once.tcl
sourceOnce cgi.tcl
sourceOnce configapp/configfile.tcl

set sidname "icsessionid"
set sid "@session@"
set urlsid ""

set UI_CONFIGFILE $env(CONFIG_ROOT)/ui_settings.conf

proc session_uid {} {
	return 1
}


#== "" wenn keine (gültige) sid in der URL
#!= "" sid der URL
proc session_urlsid { {p_tmp_sidname ""} } {
    global sid
	return $sid
}



proc session_geturlwithoutsid {} {

	global env sidname

	set queryparams ""
	catch { set queryparams $env(QUERY_STRING) }
	set queryparams [cgi_unquote_input $queryparams]
	set querylen [string length $queryparams]

	if {$querylen == 0} then {
	
		return "http://$env(HTTP_HOST)$env(SCRIPT_NAME)"
	}

	if { [string index $queryparams end] == "&" || [string index $queryparams end] == "?" } then {

		decr querylen
		set queryparams [string range $queryparams 0 $querylen]
	}

	if { [regexp {(.*)(icsessionid=@[A-Za-z0-9]*@)(.*)} $queryparams dummy prev sid post] || [regexp {(.*)(sid=@[A-Za-z0-9]*@)(.*)} $queryparams dummy prev sid post] } then {

		set prevlen [string length $prev]
		set postlen [string length $post]

		if {$prevlen > 0} then {
			
			if {[string index $prev end] == "&"} then {

				decr prevlen 2
				set prev [string range $prev 0 $prevlen]

				incr prevlen
			}
		}

		if {$postlen > 0} then {

			if {[string index $post 0] == "&"} then {

				set post [string range $post 1 end]
				decr postlen
			}
		}
		
		if { $prevlen > 0 && $postlen > 0 } then {
			set queryparams $prev&$post
		} else {
			set queryparams $prev$post
		}

		set querylen [string length $queryparams]
	}

	if {$querylen > 0} then {
		set url "http://$env(HTTP_HOST)$env(SCRIPT_NAME)?$queryparams"
	} else {
		#params bestanden nur aus der sid, die nun wegfällt.
		set url "http://$env(HTTP_HOST)$env(SCRIPT_NAME)"
	}

	return [cgi_quote_html $url]
}



proc session_requestisvalid {needed_upl} {
	return 1
}

#
#Liefert zurück, ob der Expertenmodus für den aktuellen User aktiviert ist
#
proc session_is_expert {} {
    global UI_CONFIGFILE
    
    array set ui_config ""
    catch { read_config $UI_CONFIGFILE ui_config }
    set expert_mode 0
    if { [info exists ui_config(ExpertMode)] } {
        set expert_mode [expr ( $ui_config(ExpertMode) != 0 ) && ( $ui_config(ExpertMode) != "false" ) ]
    }
    return $expert_mode
}


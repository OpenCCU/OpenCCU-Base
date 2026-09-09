namespace eval ::HomeMatic {
	namespace export Addon Script Session Util GetSerialNumber
	
	if { ![info exists rega_script] } then {
		load tclrega.so
	}
	
	if { ![info exists xmlrpc] } then  {
		load tclrpc.so
	}
}

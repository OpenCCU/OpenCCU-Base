namespace eval ::HomeMatic {
	namespace export Addon Script Session Util GetSerialNumber
	
	if { ![llength [info commands ::rega_script]] } then {
		load tclrega.so
	}
	
	if { ![llength [info commands ::xmlrpc]] } then  {
		load tclrpc.so
	}
}

#!/bin/tclsh
source once.tcl
sourceOnce cgi.tcl
sourceOnce common.tcl

#   -1: upl zur übergebenen uid nicht gefunden
#>=  0: upl des users mit der übergebenen uid
proc user_upl_ise {uid} {

	return 8
}

#== "": user zur übergebenen uid nicht gefunden
#!= "": username zu der übergebenen uid
proc user_name_ise {uid} {
	
	return "user"
}

#-1: User zum übergebenen uname nicht vorhanden
#>0: uid des uname
proc user_uid_ise {uname} {
	
	return 1
}

proc user_isExpert_ise {uid} {

	return 1
}
#======================================================================
#======================================================================
#======================================================================

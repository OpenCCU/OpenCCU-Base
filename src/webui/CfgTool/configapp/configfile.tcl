proc read_config {filename array_var} {
    upvar $array_var arr
    set fd [open $filename r]
    while { ! [eof $fd] } {
        set line [gets $fd]
        if [regexp -line {^([^= \t]+)[ \t]*=[ \t]*(.*)$} $line dummy key value] {
            set arr($key) $value
        }
    }
    close $fd
}

proc write_config {filename array_var} {
    upvar $array_var arr
    set data ""
    catch {
        set fd [open $filename r]
        set data [read $fd]
        close $fd
    }

    foreach key [array names arr] {
        if [regexp -line -indices "^$key\[ \t\]*=\[ \t\]*.*\$" $data range] {
            if { "$arr($key)" == "<<<UNSET>>>" } {
                set entry ""
            } else {
                set entry "$key = $arr($key)\n"
            }
            set data [string replace $data [lindex $range 0] [expr [lindex $range 1] + 1 ] "$entry"] 
        } else {
            if { "$arr($key)" == "<<<UNSET>>>" } continue
            
            if { [ string index $data end ] != "\x0a" } {
                append data "\n"
            }
            append data "$key = $arr($key)\n"
        }
    }
    
    set fd [open $filename w]
    puts -nonewline $fd $data
    close $fd
    
}
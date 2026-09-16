proc ::HomeMatic::GetSerialNumber { } {
  set content [::HomeMatic::Util::LoadFile "/etc/config/ids"]
  
  if { [regexp -line {SerialNumber=(.*)} $content dummy serial] } {
    return $serial
  } else {
    return INVALID
  }
}

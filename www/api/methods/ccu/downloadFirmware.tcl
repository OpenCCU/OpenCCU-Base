##
# CCU.downloadFirmware
# Laedt die neuste verfuegbare CCU Firmware herunter.
#
# Parameter:
#  
#
# Rueckgabewert: True wenn eine Firmware erfolgreich herunterladen wurde, ansonsten False.
##

# the latest version and the platform are needed for the download URL
set result 1
set error "failed to determine the latest version or the platform"
if { [catch {exec curl -s -L https://openccu.de/LATEST-VERSION.js} verstring] == 0
     && [regexp {homematic\.com\.setLatestVersion\('(.+)', 'HM-RASPBERRYMATIC'\);} $verstring match latest]
     && [catch {exec grep -m1 {^PLATFORM=} /VERSION | cut -d= -f2} platform] == 0
     && $platform != "" } {
  set url "https://github.com/openccu/openccu/releases/download/$latest/OpenCCU-$latest-$platform.zip"
  set result [catch {exec wget -q --https-only -O /usr/local/tmp/firmwareUpdateFile $url} error]
}
if { $result != 0 } {
  exec echo $error > /tmp/dwnld.error
}

if { $result == 0 } {
  jsonrpc_response true
} else {
  jsonrpc_response false
}

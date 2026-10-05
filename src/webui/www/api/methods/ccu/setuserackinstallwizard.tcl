##
# CCU.setUserAckInstallWizard
# Legt die Datei /etc/config/userprofiles/userAckInstallWizardId_USERID für den
# angemeldeten Anwender an
#
# Parameter:
#  keine (der Anwender wird über die Sitzung ermittelt)
#
# Rückgabewert: immer true
##

set script {
  var s = system.GetSessionVarStr(_session_id_);
  Write(s.StrValueByIndex(";", 0));
}
set userId [hmscript $script args]

if {[regexp {^[0-9]+$} $userId]} {
  catch {exec touch /etc/config/userprofiles/userAckInstallWizardId_$userId}
}

jsonrpc_response true

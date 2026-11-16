##
# CCU.setUserAckInstallWizard
# Legt die Datei /etc/config/userprofiles/userAckInstallWizardId_USERID für den
# angemeldeten Anwender an
#
# Parameter:
#  legacyName: [string, optional] bisheriger Name der Datei
#              userAckInstallWizard_NAME, die damit übernommen und entfernt wird
#
# Der Anwender wird über die Sitzung ermittelt.
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

  # Bisherige, namensbasierte Datei entfernen, damit sie nicht von einem
  # weiteren Anwender mit gleichem bereinigten Namen übernommen wird
  if {[info exists args(legacyName)] && [regexp {^[!-.0-~]+$} $args(legacyName)]} {
    catch {file delete /etc/config/userprofiles/userAckInstallWizard_$args(legacyName)}
  }
}

jsonrpc_response true

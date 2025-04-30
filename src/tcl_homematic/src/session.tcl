namespace eval ::HomeMatic::Session {
	namespace export LOGIN_URL RENEW_URL login logout renew
}

set ::HomeMatic::Session::LOGIN_URL "127.0.0.1/login.htm"
set ::HomeMatic::Session::RENEW_URL "127.0.0.1/pages/index.htm"

# Benutzeranmeldung.
# Versucht eine neue Session zu erstellen und den Benutzer anzumelden.
#
# <p><b>Ausnahmen</b>
# <ul>
#   <li><i>create:</i> Falls keine Sitzung erstellt werden kann</li>
#   <li><i>id:</i> Falls die erzeugte Session-Id ung&uuml;ltig ist</li>
#   <li><i>credentials:</i> Falls Benutzername bzw. Passwort falsch sind</i></li>
# </ul>
# </p>
#
# @param username Benutzername
# @param password Passwort
# @return Session-Id
proc ::HomeMatic::Session::login { username password } {
	error "not implemented"
}

proc ::HomeMatic::Session::logout { sessionId } {
	error "not implemented"
}

proc ::HomeMatic::Session::renew { sessionId } {
	error "not implemented"
}

proc ::HomeMatic::Session::isValid { sessionId } {
}



/**
 * homematic.js
 **/
  
/**
 * Stellt eine Anfrage an die HomeMatic Zentrale
 * Der Parameter callback ist optional. Wird er weggelassen, so ist die Anfrage synchron.
 **/
homematic = function(method, params, callback)
{
  var _params_ = params;
  if (!_params_) { _params_ = { }; }
  _params_["_session_id_"] = getSessionId();
  
  if (typeof(callback) != "undefined") 
  {
    var _callback_ = callback;
    return jsonrpc(homematic.URL, method, _params_, function(response) {
      _callback_(homematic._checkResponse(response), response.error);
    });
  }
  else
  {
    return homematic._checkResponse(jsonrpc(homematic.URL, method, _params_));
  }
};

/**
 * URL zur HomeMatic JSON API
 **/
homematic.URL = "/api/homematic.cgi";

/**
 * Prüft die JSON-Antwort ud liefert deren Resultat
 **/
homematic._checkResponse = function(response)
{
  if (null === response.error)
  {
    return response.result;
  }
  else
  {
    // handle errors
    return null;
  }
};

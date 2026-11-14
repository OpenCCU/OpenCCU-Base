/**
 * jsonrpc.js
 * JSON-RPC
 *
 * JSON-RPC Spezifikation: http://json-rpc.org/wiki/specification
 *
 * Autor: Falk Werner
 **/

/**
 * Allgemeine JSON-RPC Fehler
 **/
JSONRPC_ERROR = 
{
	INVALID_JSON: 
	{
		"result": null,
    "error" : {"code": 200, "text": "invalid response"},
    "id"    : null
	}
}; 
 
/**
 * 
 **/
jsonrpc_check = function(response)
{
	if ((null       === response)                   || 
			("undefined" == typeof(response["result"])) ||
			("undefined" == typeof(response["error"])))
	{
		response = JSONRPC_ERROR.INVALID_JSON;
	}
	
	return response;
};

/**
 * JSON-RPC Aufruf.
 * Falls der Parameter callback nicht angegeben ist, handelt es sich um
 * einen asynchronen Aufruf.
 **/
jsonrpc = function(url, method, params, callback)
{
  if (typeof(callback) != "undefined") { return jsonrpc_async(url, method, params, callback); }
  else { return jsonrpc_sync(url, method, params); }
};

/**
 * Asynchroner JSON-RPC Aufruf.
 **/
jsonrpc_async = function(url, method, params, callback)
{
	var user_callback = callback;
	var xhr           = XMLHttpRequest_create();
	
	xhr.open("POST", url, true);
	xhr.setRequestHeader("Content-Type", "application/json");
	xhr.onreadystatechange = function()
	{
		if (4 == xhr.readyState)
		{
			var result = null;
			if ((0 === xhr.status) || (200 == xhr.status))
			{
				try        { result = eval("(" + xhr.responseText + ")"); }
				catch (ex) { result = null; }
			}
			if (user_callback) { user_callback(jsonrpc_check(result)); }
		}
	};
	xhr.send(Object.toJSON({"version": "1.1", "method": method, "params": params }));
  
  return true;
};

/**
 * Sychroner JSON-RPC Aufruf.
 **/
jsonrpc_sync = function(url, method, params)
{
	var result = null;
	var xhr    = XMLHttpRequest_create();
	
	xhr.open("POST", url, false);
	xhr.setRequestHeader("Content-Type", "application/json");
	xhr.send(Object.toJSON({"version": "1.1", "method": method, "params": params }));
  if ((0 === xhr.status) || (200 == xhr.status))
  {
    try        { result = eval("(" + xhr.responseText + ")"); }
    catch (ex) { result = null; }
  }
	return jsonrpc_check(result);
};

/**
 * JSON-RPC Aufruf ohne Rückgabewert.
 **/
jsonrpc_notify = function(url, method, params)
{
	var xhr           = XMLHttpRequest_create();
	xhr.open("POST", url, true);
	xhr.setRequestHeader("Content-Type", "application/json");
	xhr.send(Object.toJSON({"version": "1.1", "method": method, "params": params}));
};

/**
 * xmlrpc.js
 * JavaScript XML-RPC Client
 *
 * XML-RPC Spezigiaktion: http://www.xmlrpc.com/
 *
 * Einschänkungen:
 *   - Datentyp "dateTime.iso8601" ist nicht verfügbar
 *   - Datentyp "base64" ist nicht verfügbar
 **/

/**
 * Allgemeine XML-RPC-Fehler
 **/ 
XMLRPC_ERROR = {
	INVALID_RESPONSE:
	{
		faultCode  : 100,
		faultString: "invalid response"
	}
};
 
/**
 * Konvertiert eine JavaScript-Zeichenkette in eine XML-Zeichenkette.
 **/
xmlrpc_xmlEscape = function(str)
{
	str = str.replace(/&/g, "&amp;");
	str = str.replace(/</g, "&lt;");
	str = str.replace(/>/g, "&gt;");
	str = str.replace(/'/g, "&apos;");
	str = str.replace(/"/g, "&quot;");
	
	return str;
};

/**
 * Liefert den ersten Kindknoten mit dem angegebenen Namen.
 **/
node_getChildNode = function(node, name)
{
	var childNodes = node.childNodes;
	for (var i = 0, len = childNodes.length; i < len; i++)
	{
		if (childNodes[i].nodeName == name) { return childNodes[i]; }
	}
	return null;
};

/**
 * Liefert alle direkten Kind-Elemente.
 **/
node_getChildElements = function(node, name)
{
	var result = new Array();
	var childNodes = node.childNodes;
	for (var i = 0, len = childNodes.length; i < len; i++)
	{
		if (childNodes[i].nodeType == 1) { result.push(childNodes[i]); }
	}
	return result;
};

/**
 * Liefert das erste Kind-Element.
 **/
node_getFirstChildElement = function(node)
{
	var childNodes = node.childNodes;
	for (var i = 0, len = childNodes.length; i < len; i++)
	{
		if (childNodes[i].nodeType == 1) { return childNodes[i]; }
	}
	return null;
};
 
/**
 * Erstellt eine XML-RPC-Anfrage.
 **/
xmlrpc_createRequest = function(method, params) 
{
	var request = "<?xml version=\"1.0\"?>";
	
	request += "<methodCall>";
	request += "<methodName>" + method + "</methodName>";
	
	if (null !== params)
	{
		request += "<params>";
		for (var i = 0, len = params.length; i < len; i++)
		{
			request += "<param>" + xmlrpc_writeValue(params[i]) + "</param>";
		}	
		request += "</params>";
	}
	
	request += "</methodCall>";
	
	return request;
};
 
/**
 * Liefert einen XML-RPC-Wert
 **/
xmlrpc_writeValue = function(value)
{
	var result = "<value>";
	
	if (typeof(value) == "boolean")
	{ 
		if (true === value) { result += "<boolean>1</boolean>"; }
		else               { result += "<boolean>0</boolean>"; }
	}
	else if ((typeof(value) == "number") && (value == parseInt(value)))
	{ 
		result += "<i4>" + value + "</i4>"; 
	}
	else if (typeof(value) == "number")
	{
		result += "<double>" + value + "</double>"; 
	}
	else if (typeof(value) == "string")
	{ 
		result += xmlrpc_xmlEscape(value); 
	}
	else if (Object.isArray(value))
	{
		result += "<array><data>";
		for (var i = 0, len = value.length; i < len; i++)
		{
			result += xmlrpc_writeValue(value[i]);
		}
		result += "</data></array>";
  }
	else if (typeof(value) == "object")
	{
		result += "<struct>";
		for (var name in value)
		{
			result += "<member>";
			result += "<name>" + name + "</name>";
			result += xmlrpc_writeValue(value[name]);
			result += "</member>";
		}
		result += "</struct>";
  }

	result += "</value>";
	
	return result;
};

/**
 * Parst eine XML-RPC-Anfrage.
 **/
xmlrpc_parse = function(responseXml)
{
	var result;
	var methodResponse;
  
	try
	{
		methodResponse = responseXml.getElementsByTagName("methodResponse")[0];
		var params     = node_getChildNode(methodResponse, "params");
		var param      = node_getChildNode(params, "param");
		result         = xmlrpc_parseValue(node_getFirstChildElement(param));
	}
	catch (ex)
	{
		try
		{
			methodResponse = responseXml.getElementsByTagName("methodResponse")[0];
			var fault      = node_getChildNode(methodResponse, fault);
			result         = xmlrpc_parseValue(node_getFirstChildElement(fault));
		}
		catch (ex2)
		{
			result = XMLRPC_ERROR.INVALID_RESPONSE;
		}
	}
	
	return result;
};
 
/**
 * Parst einen XML-RPC-Wert.
 **/
xmlrpc_parseValue = function(valueNode)
{
	var typeNode = node_getFirstChildElement(valueNode);
	
	if (typeNode !== null)
	{
		if      (typeNode.nodeName == "boolean") { return parseBoolean(typeNode.firstChild.data); }
		else if (typeNode.nodeName == "int")     { return parseInt(typeNode.firstChild.data); }
		else if (typeNode.nodeName == "i4")      { return parseInt(typeNode.firstChild.data); }
		else if (typeNode.nodeName == "double")  { return parseFloat(typeNode.firstChild.data); }
		else if (typeNode.nodeName == "string")  { return typeNode.firstChild.data; }
		else if (typeNode.nodeName == "array")
		{
			var result = new Array();
			var data   = node_getChildNode(typeNode, "data");
			var values = node_getChildElements(data, "value");
			for (var i = 0, len = values.length; i < len; i++)
			{
				result.push(xmlrpc_parseValue(values[i]));
			}
			return result;
		}
		else if (typeNode.nodeName == "struct")
		{
			var result = {};
			var member = node_getChildElements(typeNode, "member");
			for (var i = 0, len = member.length; i < len; i++)
			{
				var member_name  = node_getChildNode(member[i], "name").firstChild.data;
				var member_value = node_getChildNode(member[i], "value");
				result[member_name] = xmlrpc_parseValue(member_value);
			}
			return result;
		}
		else { return null; }
	}
	else if (null !== valueNode.firstChild) { return valueNode.firstChild.data; }
	else return "";	
};
 
/**
 * Asynchrone XML-RPC-Anfrage.
 **/
xmlrpc = function(url, method, params, callback)
{
	var user_callback = callback;
	var xhr           = XMLHttpRequest_create();
	
	xhr.open("POST", url, true);
	xhr.setRequestHeader("Content-Type", "text/xml");
	xhr.onreadystatechange = function()
	{
		if (4 == xhr.readyState)
		{
			var result = null;
			if ((0 === xhr.status) || (200 == xhr.status))
			{
				result = xhr.responseXML;
			}
			if (user_callback) { user_callback(xmlrpc_parse(result)); }
		}
	};
	xhr.send(xmlrpc_createRequest(method, params));
};

parseBoolean = function(value)
{
  value = value.toLowerCase();
  
  if (("1" == value) || ("true" == value) || ("yes" == value)) { return true;  }
  else                                                         { return false; }
};
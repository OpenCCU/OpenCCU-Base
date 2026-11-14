/**
 * rega.js
 * Zugriff auf den ise ReGa Webserver.
 **/
 
/**
 * @fileOverview ?
 * @author Falk Werner (eQ-3)
 **/

/**
 * @class
 **/ 
ReGa = {

  /**
   * 1) ReGa arbeitet mit Latin-1 Zeichencodierung (ISO-8859-1)
   * 2) Anfragen über das XMLHttpRequest-Objekt sind i.d.R. UTF-8-codiert
   *
   * ==> Codierung der Anfrage:
   *     - escape() wandelt UTF-8 nach ASCII um 
   *       (Latin-1 Sonderzeichen werden durch Escape-Sequenzen ersetzt)
   *     - Anfrage wird in <prototypejs><![CDATA[ ... ]]><prototypejs>
   *       verpackt
   *
   * ==> Decodierung erfolgt durch den ReGa Webserver
   **/
  encode: function(data) 
  {
    return "<prototypejs><![CDATA[" + escape(data) + "]]></prototypejs>";
  },

  /**
   * Escapes a value for a double-quoted string literal of a ReGa script,
   * e.g. pb += 'string value = "' + ReGa.escapeString(value) + '";';
   * ReGa removes the escaping again when it parses the literal.
   **/
  escapeString: function(value)
  {
    return String(value).replace(/\\/g, "\\\\").replace(/"/g, "\\\"").replace(/\r/g, "\\r").replace(/\n/g, "\\n");
  }
};

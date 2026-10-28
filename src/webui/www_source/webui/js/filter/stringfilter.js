/**
 * stringfilter.js
 **/
 
/**
 * Checks if a text is a valid regular expression.
 **/
const isValidRegex = function(possiblyValidRegex)
{
  try
  {
    new RegExp(possiblyValidRegex);
  }
  catch(err)
  {
    return false;
  }

  return true;
}

/**
 * Checks if a text matches the filter as entered by the user, used as a
 * case-insensitive regular expression. Callers also check the filter as
 * plain text, so text with special characters like "(" or "+" still matches.
 **/
const matchesFilterRegex = function(text, filter)
{
  if ((typeof filter != "string") || (filter === "")) { return false; }
  if (!isValidRegex(filter)) { return false; }

  return new RegExp(filter, "i").test(text);
}

/**
 * Konstrukor. Filter für Zeichenketten
 **/
StringFilter = function(name, callback)
{
  /*####################*/
  /*# Private Elemente #*/
  /*####################*/
  
  
  var m_name     = name;
  var m_value    = "";
  var m_callback = callback;
  
  var m_id     = name.replace(/\,/g, "_");
  var m_textId = m_id + "Text";

  var isSet = function()
  {
    return (m_value !== "");
  };
  
  /*########################*/
  /*# Öffentliche Elemente #*/
  /*########################*/
  
  /**
   * Liefert den HTML-Code des Filters
   **/
  this.getHTML = function(colspan)
  {
    var _colspan_ = colspan;
    if (typeof(_colspan_) == "undefined") { _colspan_ = 1; }
    
    return StringFilter.TEMPLATE.process({
      colspan: _colspan_,
      name   : m_name,
      id     : m_id,
      textId : m_textId,
      isSet  : isSet(),
      value  : m_value
    });
  };
  
  /**
   * Zeigt den Filter an.
   **/
  this.show = function()
  {
    var filterElement = $(m_id);
    var textElement = $(m_textId);

    if ((typeof oFilterStorage != "undefined")) {
      if (oFilterStorage[m_name] && oFilterStorage[m_name] != null) {
        textElement.value = oFilterStorage[name];
      }
    } else {
      oFilterStorage = {};
    }

    if (typeof oTimerStorage == "undefined") {
      oTimerStorage = {};
    }

    if ((filterElement) && (textElement))
    {
      filterElement.show();
      textElement.focus();
    }
  };
  
  /**
   * Prüft, ob der Filter auf einen Text zutrifft
   **/
  this.match = function(text)
  {
    if (false === isSet()) { return true; }
    
    var patternList = m_value.toLowerCase().split("|");
    //var patternList = m_value.split("|");
    if (matchesFilterRegex(text, m_value)) { return true; }

    text            = text.toLowerCase();
    
    for (var i = 0, len = patternList.length; i < len; i++)
    {
      if (0 <= text.indexOf(patternList[i])) { return true; }
    }
    
    return false;
  };
  
  /**
   * Schließt den Filter und ruft dei Callback-Funktion auf
   **/
  this.set = function()
  {
    if ($(m_textId)) {
      var storageTime = 300000; // 5 Minutes
      m_value = $(m_textId).value;
      // Store the search term for 5 Minutes
      // After that, check if another search term exists.
      if (typeof oFilterStorage != "undefined") {
        if (oFilterStorage[m_name] != m_value) {
          oFilterStorage[m_name] = m_value;
          if (oTimerStorage[m_name]) {
            clearTimeout(oTimerStorage[m_name]);
          }
          oTimerStorage[m_name] = window.setTimeout(function() {
            oFilterStorage[m_name] = null;
          },storageTime);
        } else {
          // Restart the timer
          clearTimeout(oTimerStorage[m_name]);
          oTimerStorage[m_name] = window.setTimeout(function() {
            oFilterStorage[m_name] = null;
          },storageTime);
        }
      }
    }
    if ($(m_id))     { $(m_id).hide(); }
    if (m_callback)  { m_callback(); }
  };
  
  /**
   * Schließt den Filter ohne Änderungen zu übernehmen
   **/
  this.close = function()
  {
    if ($(m_textId)) { $(m_textId).value = m_value; }
    if ($(m_id)) { $(m_id).hide(); }
  };
  
  /**
   * Setzt den Filter zurück
   **/
  this.reset = function()
  {
    m_value = "";
    this.close();
  };

  /**
   * Prüft, ob Enter oder ESC gedrückt wurde und schließt den Filter entsprechend
   */
  this.checkEnterEsc = function(key)
  {
    switch (key) {
      case 13:
        this.set();
        break;
      case 27:
        this.close();
        break;
    }
  };

  /*###################*/
  /*# Initialisierung #*/
  /*###################*/
  
  this.reset();
  
};

StringFilter.TEMPLATE = TrimPath.parseTemplate(STRINGFILTER_JST);

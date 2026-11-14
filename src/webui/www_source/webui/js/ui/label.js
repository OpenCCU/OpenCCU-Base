/**
 * label.js
 **/

/**
 * Einzeliges Textfeld
 **/
UI.Label = Class.create(UI.Component, {

  /**
   * Konstruktor. Erstellt ein einzeiliges Textfeld.
   **/ 
  initialize: function()
  {
    this.m_isEnabled = true;

    this.m_element = document.createElement("div");
    this.m_element.className = UI.Label.CLASS_NAME;
    this.m_element.appendChild(document.createTextNode(UI.Label.DEFAULT_TEXT));
  },

  /**
   * Setzt den Text des Labels
   **/
  setText: function(text)
  {
    this.m_element.innerHTML = "";
    this.m_element.appendChild(document.createTextNode(text));
    return this;
  },
  
  /**
   * Setzt die Breite des Labels.
   * Neben numerischen Angaben ist auch der Wert "auto" erlaubt.
   **/
  setWidth: function(width)
  {
    if (width != "auto") { width = parseInt(width) + "px"; }
    Element.setStyle(this.m_element, { width: width });
    return this;
  },
  
  /**
   * Setzt die Höhe des Labels.
   * Neben numerischen Angaben ist auch der Wert "auto" erlaubt.
   * Es wird immer auch die Zeilenhöhe gesetzt!
   **/
  setHeight: function(height)
  {
    var lineHeight = "100%";
    if (height != "auto") 
    {
      height     = parseInt(height) + "px"; 
      lineHeight = height;
    }
    
    Element.setStyle(this.m_element, {
      height    : height,
      lineHeight: lineHeight});
    return this;
  }
  
}); 

/**
 * Konstanten
 **/
UI.Label.CLASS_NAME   = "UILabel";
UI.Label.DEFAULT_TEXT = "UI.Label";
